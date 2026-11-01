#pragma once

#include "ECS/ECS_Entity.hpp"

struct RouletteComponent
{
    static constexpr uint32_t component_type = (1 << 16) + 3;
    static constexpr bool notify_reparent = false;
    static RedMagic::indexed_vector<RouletteComponent> storage;

    RedMagic::ECS::Entity owner;

    int result = -1;
    bool isSpinning = false;
    bool hasResult = false;
};
