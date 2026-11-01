#pragma once

#include "ECS/ECS_Entity.hpp"

struct PlayerComponent
{
    static constexpr uint32_t component_type = (1 << 16) + 1;
    static constexpr bool notify_reparent = false;
    static RedMagic::indexed_vector<PlayerComponent> storage;

    RedMagic::ECS::Entity owner;

    int money = 1000;

    int totalGains = 0;
    int totalLosses = 0;
};