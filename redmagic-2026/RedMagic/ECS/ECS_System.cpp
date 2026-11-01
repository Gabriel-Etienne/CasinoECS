
#include "ECS_System.hpp"

#include <iostream>
#include <raylib.h>

#include <utility>

#include "ECS_Entity.hpp"

using namespace RedMagic;

static void ReorderECS(indexed_vector<std::shared_ptr<ECS::System>, uint16_t>& the_ecs) {
    // You have not had the math course to handle this ₍ᐢ.ˬ.ᐢ₎♡~
    // We will return to this when we do the course on graphs and topological sorting.
}

namespace {
    enum HOOK : std::size_t {
        DRAW_START,
        DRAW_END,
        LOOP_START,
        LOOP_END,
        INPUT,
        HOOK_MAX
    };
}

static std::array<std::function<void()>, static_cast<size_t>(HOOK_MAX)> hooks;
static auto target_delta_time = std::chrono::duration<double>{};
bool ECS::System::dirty = false;

void ECS::System::RegisterOnDrawStart(std::function<void()> func) {
    hooks[DRAW_START] = std::move(func);
}
void ECS::System::RegisterOnDrawEnd(std::function<void()> func) {
    hooks[DRAW_END] = std::move(func);
}
void ECS::System::RegisterOnLoopStart(std::function<void()> func) {
    hooks[LOOP_START] = std::move(func);
}
void ECS::System::RegisterOnLoopEnd(std::function<void()> func) {
    hooks[LOOP_END] = std::move(func);
}
void ECS::System::RegisterInput(std::function<void()> func) {
    hooks[INPUT] = std::move(func);
}
void ECS::System::SetLoopCadence(const float fps) {
    if (fps <= 0.0) target_delta_time = {};
    else target_delta_time = std::chrono::duration<double>(1.0) / fps;
}

ECS::duration_pair ECS::System::UpdateAll(const std::chrono::duration<double> delta_time) {
    const auto start = std::chrono::steady_clock::now();
    bool draw_started = false;

    // If we modified which systems are running, some order tweaking may be required
    if (dirty) {
        ReorderECS(storage);
        dirty = false;
    }

    // Run the hooks if they exist
    hooks[LOOP_START] ? hooks[LOOP_START]() : void();
    hooks[INPUT] ? hooks[INPUT]() : void();

    // Update all systems, delay the initialization of the rendering if possible
    for (const auto& system : storage) {
        if (not draw_started and system->IsRenderer()) {
            draw_started = true;
            hooks[DRAW_START]();
        }
        system->Update(delta_time);
    }

    // If any system did rendering, we need to conclude the rendering before concluding the loop
    if (draw_started) {
            hooks[DRAW_END]();
    }
    hooks[LOOP_END] ? hooks[LOOP_END]() : void();

    // If any system was removed, we need to reflect that
    dirty = storage.collect_all_marks();

    // If any entity and/or component was removed we need to handle that too
    Entity::CollectAllChildren();

    // Calculate all timing information
    const auto end = std::chrono::steady_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    // We can never wait negative time
    const auto unadjusted_time_until_next_frame = std::max(
        std::chrono::duration<double>{},
        target_delta_time - time_taken
    );
    // We can never wait negative time, but if the previous frame was slow we can try and be slightly faster than normal
    const auto adjusted_time_until_next_frame =
        std::max(
            std::chrono::duration<double>{},
            unadjusted_time_until_next_frame + std::min(
                std::chrono::duration<double>{},
                target_delta_time - delta_time
            )
        );

    // NOTE: Waiting on MicroSlop Windows tends to be... sloppy. While this algo makes sense in terms of math — and is
    // within tens of microseconds of accuracy on Linux, MacOS, and OpenBSD — on Windows it can be off by milliseconds.
    return std::make_pair(adjusted_time_until_next_frame, time_taken + adjusted_time_until_next_frame);
}
