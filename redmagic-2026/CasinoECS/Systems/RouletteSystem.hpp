
#pragma once

#include <chrono>
#include <cstdint>
#include <random>
#include <vector>

#include "ECS/ECS_System.hpp"

#include "../Components/PlayerComponent.hpp"
#include "../Components/BetComponent.hpp"
#include "../Components/RouletteComponent.hpp"

#include "../Utils/RouletteUtils.hpp"

class RouletteSystem final : public RedMagic::ECS::System
{
private:
    std::mt19937 rng{std::random_device{}()};

    int GenerateNumber()
    {
        std::uniform_int_distribution<int> distribution(0, 36);
        return distribution(rng);
    }

public:

    void Update(const std::chrono::duration<double> delta_time) override
    {
        auto players = PlayerComponent::storage.span();
        auto bets = BetComponent::storage.span();
        auto roulettes = RouletteComponent::storage.span();

        if (players.empty() || bets.empty() || roulettes.empty())
            return;

        PlayerComponent& player = players[0];
        BetComponent& bet = bets[0];
        RouletteComponent& roulette = roulettes[0];

        if (!roulette.isSpinning)
            return;

        if (bet.amount <= 0 || bet.amount > player.money)
        {
            roulette.isSpinning = false;
            return;
        }

        roulette.result = GenerateNumber();
        roulette.hasResult = true;

        BetColor resultColor = GetNumberColor(roulette.result);

        if (resultColor == bet.color)
        {
            int gain = bet.amount;

            if (bet.color == BetColor::Green)
                gain *= 35;

            player.money += gain;
            player.totalGains += gain;
        }
        else
        {
            player.money -= bet.amount;
            player.totalLosses += bet.amount;
        }

        // Adapter automatiquement la mise à l'argent restant.
        if (bet.amount > player.money)
            bet.amount = player.money;

        roulette.isSpinning = false;
    }

    std::vector<uint32_t> ReadComponentIDs() const override
    {
        return {
            PlayerComponent::component_type,
            BetComponent::component_type,
            RouletteComponent::component_type
        };
    }

    std::vector<uint32_t> WriteComponentIDs() const override
    {
        return {
            PlayerComponent::component_type,
            BetComponent::component_type,
            RouletteComponent::component_type
        };
    }

    bool IsRenderer() const override
    {
        return false;
    }
};
