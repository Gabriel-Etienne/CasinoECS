//
// Created by archivist on 2/13/26.
//

#ifndef GARDEN_CAMERACOMPONENT_HPP
#define GARDEN_CAMERACOMPONENT_HPP
#include <cstdint>

#include "raylib.h"
#include "indexed_vector.hpp"
#include "ECS/ECS_Entity.hpp"

namespace RedMagic::ECS {
    struct CameraComponent {
        /**
         * Every component needs this
         */
        static constexpr bool notify_reparent = false;
        /**
         * Every component needs this
         */
        static constexpr uint32_t component_type = 1;
        /**
         * Every component needs this
         */
        static indexed_vector<CameraComponent> storage;
        /**
         * Every component needs this
         */
        Entity owner;
        /**
         * The camera information (see: Raylib).
         */
        Camera2D camera;
    };
}

#endif //GARDEN_CAMERACOMPONENT_HPP