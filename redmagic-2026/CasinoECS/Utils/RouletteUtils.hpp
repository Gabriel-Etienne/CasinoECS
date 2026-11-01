#pragma once

#include "../Components/BetComponent.hpp"

inline BetColor GetNumberColor(int number)
{
    if (number == 0)
        return BetColor::Green;

    switch (number)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
        case 12:
        case 14:
        case 16:
        case 18:
        case 19:
        case 21:
        case 23:
        case 25:
        case 27:
        case 30:
        case 32:
        case 34:
        case 36:
            return BetColor::Red;

        default:
            return BetColor::Black;
    }
}