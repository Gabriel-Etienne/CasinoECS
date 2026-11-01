
#include <random>
#include <thread>
#include <chrono>

#include "raylib.h"

#include "ECS/ECS_Entity.hpp"
#include "ECS/ECS_System.hpp"

#include "Components/PlayerComponent.hpp"
#include "Components/BetComponent.hpp"
#include "Components/RouletteComponent.hpp"

#include "Systems/CasinoRenderSystem.hpp"
#include "Systems/CasinoInputSystem.hpp"
#include "Systems/RouletteSystem.hpp"

std::mt19937 rng;

int main()
{
    using namespace RedMagic::ECS;
    rng.seed(std::random_device{}());

    InitWindow(1280, 720, "CasinoECS");

    // SYSTEMS
    System::Initialise<CasinoInputSystem>();
    System::Initialise<RouletteSystem>();
    System::Initialise<CasinoRenderSystem>();

    System::RegisterOnDrawStart(BeginDrawing);
    System::RegisterOnDrawEnd(EndDrawing);
    System::SetLoopCadence(0.0f);

    // ENTITIES
    const auto player = Entity::CreateEntity();
    player.CreateComponent<PlayerComponent>();

    const auto bet = Entity::CreateEntity();
    bet.CreateComponent<BetComponent>();

    const auto roulette = Entity::CreateEntity();
    roulette.CreateComponent<RouletteComponent>();


    std::chrono::duration<double> delta_time{};

    while (!WindowShouldClose())
    {
        auto [wait_time, next_delta_time] =
            System::UpdateAll(delta_time);

        std::this_thread::sleep_for(wait_time);

        delta_time = next_delta_time;
    }

    CloseWindow();

    return 0;
}
