#pragma once

#include "ECS/ECS_Entity.hpp"

enum class BetColor
{
    Red,
    Black,
    Green
};

struct BetComponent
{
    static constexpr uint32_t component_type = (1 << 16) + 2;
    static constexpr bool notify_reparent = false;
    static RedMagic::indexed_vector<BetComponent> storage;

    RedMagic::ECS::Entity owner;

    int amount = 0;
    BetColor color = BetColor::Red;
};
