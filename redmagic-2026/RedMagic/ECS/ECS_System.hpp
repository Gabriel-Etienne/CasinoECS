//
// Created by archivist on 9/26/26.
//

#ifndef REDMAGIC_SYSTEM_HPP
#define REDMAGIC_SYSTEM_HPP
#include <chrono>
#include <functional>

#include "indexed_vector.hpp"

namespace RedMagic::ECS {
    using duration_pair = std::pair<std::chrono::duration<double>, std::chrono::duration<double>>;

    class System {
        static indexed_vector<std::shared_ptr<System>, uint16_t> storage;
        /**
         * Whether the system list was changed
         */
        static bool dirty;
    public:
        /**
         * Creates and adds a system to the ECS
         * @tparam T They type of the system
         * @param args constructor arguments
         * @return The system_id under which it was added and a shared_ptr to the system
         */
        template<std::derived_from<System> T>
        static std::pair<
            uint16_t,
            std::shared_ptr<T>
        > Initialise(auto&&... args) {
            auto sys = std::make_shared<T>(std::forward<decltype(args)>(args)...);
            auto idx = storage.emplace_back(sys);
            dirty = true;
            return std::make_pair(idx, std::move(sys));
        }

        /**
         * Marks the system to be removed at the end of the frame
         * @param system_id The system_id of the system to remove
         */
        static void RemoveSystem(const uint16_t system_id) {
            storage.mark_for_discard(system_id);
        }

        virtual void Update(std::chrono::duration<double> delta_time) = 0;
        [[nodiscard]] virtual std::vector<uint32_t> ReadComponentIDs() const = 0;
        [[nodiscard]] virtual std::vector<uint32_t> WriteComponentIDs() const = 0;
        [[nodiscard]] virtual bool IsRenderer() const { return false; }
        virtual ~System() = default;

        /**
         * Adds a callback when the first system asks to render
         */
        static void RegisterOnDrawStart(std::function<void()> func);
        /**
         * Adds a callback when the rendering is completed
         */
        static void RegisterOnDrawEnd(std::function<void()> func);
        /**
         * Adds a callback when the loop execution starts
         */
        static void RegisterOnLoopStart(std::function<void()> func);
        /**
         * Adds a callback when the loop execution ends
         */
        static void RegisterOnLoopEnd(std::function<void()> func);
        /**
         * Adds a callback when the loop should process input
         */
        static void RegisterInput(std::function<void()> func);
        /**
         * Sets the FPS calculation.
         */
        static void SetLoopCadence(float fps);

        /**
         * @return The time we will need to wait to fit out cadence and the delta_time we will need to provide for the next frame.
         */
        static duration_pair UpdateAll(std::chrono::duration<double>);
    };
}


#endif //REDMAGIC_SYSTEM_HPP
