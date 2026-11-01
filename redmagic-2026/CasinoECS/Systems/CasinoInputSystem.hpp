
#pragma once

#include <chrono>
#include <algorithm>
#include <vector>

#include "raylib.h"
#include "ECS/ECS_System.hpp"

#include "../Components/PlayerComponent.hpp"
#include "../Components/BetComponent.hpp"
#include "../Components/RouletteComponent.hpp"

class CasinoInputSystem final : public RedMagic::ECS::System
{
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

        // Game Over
        if (player.money <= 0)
        {
            if (IsKeyPressed(KEY_R))
            {
                player.money = 1000;

                player.totalGains = 0;
                player.totalLosses = 0;

                bet.amount = 0;
                bet.color = BetColor::Red;

                roulette.result = -1;
                roulette.isSpinning = false;
                roulette.hasResult = false;
            }

            return;
        }

        // Aucun contrôle pendant le tirage ( evite les bugs )
        if (roulette.isSpinning)
            return;

        // Changement de couleur
        if (IsKeyPressed(KEY_RIGHT))
        {
            int color = static_cast<int>(bet.color);
            color = (color + 1) % 3;
            bet.color = static_cast<BetColor>(color);
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            int color = static_cast<int>(bet.color);
            color = (color + 2) % 3;
            bet.color = static_cast<BetColor>(color);
        }

        // Augmenter la mise sans dépasser l'argent disponible ( logique + pas frustration )
        if (IsKeyPressed(KEY_UP))
        {
            bet.amount = std::min(bet.amount + 10, player.money);
        }

        // Diminuer la mise
        if (IsKeyPressed(KEY_DOWN))
        {
            bet.amount = std::max(0, bet.amount - 10);
        }

        // Lancer la roulette
        if (IsKeyPressed(KEY_SPACE))
        {
            if (bet.amount > 0 && bet.amount <= player.money)
            {
                roulette.isSpinning = true;
                roulette.hasResult = false;
            }
        }
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
            BetComponent::component_type,
            RouletteComponent::component_type
        };
    }

    bool IsRenderer() const override
    {
        return false;
    }
};
