
#pragma once

#include <chrono>
#include <vector>

#include "raylib.h"
#include "ECS/ECS_System.hpp"

#include "../Components/PlayerComponent.hpp"
#include "../Components/BetComponent.hpp"
#include "../Components/RouletteComponent.hpp"

#include "../Utils/RouletteUtils.hpp"

class CasinoRenderSystem final : public RedMagic::ECS::System
{
public:
    void Update(const std::chrono::duration<double> delta_time) override
    {
        auto players = PlayerComponent::storage.span();
        auto bets = BetComponent::storage.span();
        auto roulettes = RouletteComponent::storage.span();

        ClearBackground(Color{ 25, 28, 35, 255 });

        if (players.empty() || bets.empty() || roulettes.empty())
            return;

        const PlayerComponent& player = players[0];
        const BetComponent& bet = bets[0];
        const RouletteComponent& roulette = roulettes[0];

        // Titre
        DrawText("CASINO - ROULETTE", 40, 30, 40, GOLD);

        // Informations du joueur
        DrawText(TextFormat("Money: %d", player.money),
                 40, 120, 30, WHITE);

        DrawText(TextFormat("Bet: %d", bet.amount),
                 40, 165, 30, WHITE);

        // Statistiques de la partie
        DrawText("GAME STATISTICS", 40, 225, 22, GOLD);

        DrawText(TextFormat("Total gains: +%d", player.totalGains),
                 40, 260, 20, GREEN);

        DrawText(TextFormat("Total losses: -%d", player.totalLosses),
                 40, 290, 20, RED);

        int netResult = player.totalGains - player.totalLosses;

        Color netColor = netResult >= 0 ? GREEN : RED;

        DrawText(TextFormat("Net result: %s%d",
                            netResult >= 0 ? "+" : "",
                            netResult),
                 40, 325, 20, netColor);

        // Couleur sélectionnée
        DrawText("Selected color:", 40, 375, 22, LIGHTGRAY);

        Color red = RED;
        Color black = BLACK;
        Color green = GREEN;

        Color selectedOutline = GOLD;

        DrawRectangle(40, 415, 100, 50, red);
        DrawRectangle(160, 415, 100, 50, black);
        DrawRectangle(280, 415, 100, 50, green);

        if (bet.color == BetColor::Red)
            DrawRectangleLinesEx(Rectangle{ 35, 410, 110, 60 }, 4, selectedOutline);

        if (bet.color == BetColor::Black)
            DrawRectangleLinesEx(Rectangle{ 155, 410, 110, 60 }, 4, selectedOutline);

        if (bet.color == BetColor::Green)
            DrawRectangleLinesEx(Rectangle{ 275, 410, 110, 60 }, 4, selectedOutline);

        DrawText("RED", 65, 430, 20, WHITE);
        DrawText("BLACK", 175, 430, 20, WHITE);
        DrawText("GREEN", 295, 430, 20, WHITE);

        // Commandes
        DrawText("LEFT / RIGHT : Change color", 40, 500, 18, LIGHTGRAY);
        DrawText("UP / DOWN : Change bet", 40, 530, 18, LIGHTGRAY);
        DrawText("SPACE : Spin", 40, 560, 18, LIGHTGRAY);

        // Résultat
        if (roulette.isSpinning)
        {
            DrawText("Spinning...", 40, 610, 35, GOLD);
        }
        else if (roulette.hasResult)
        {
            BetColor resultColor = GetNumberColor(roulette.result);

            Color displayColor = WHITE;

            if (resultColor == BetColor::Red)
                displayColor = RED;
            else if (resultColor == BetColor::Black)
                displayColor = LIGHTGRAY;
            else
                displayColor = GREEN;

            DrawText("RESULT:", 40, 600, 25, WHITE);

            DrawText(TextFormat("%d", roulette.result),
                     40, 635, 55, displayColor);

            if (resultColor == BetColor::Red)
                DrawText("RED", 110, 650, 25, RED);
            else if (resultColor == BetColor::Black)
                DrawText("BLACK", 110, 650, 25, LIGHTGRAY);
            else
                DrawText("GREEN", 110, 650, 25, GREEN);
        }

        // Panneau des probabilités et des gains
        DrawRectangle(760, 140, 470, 430, Color{ 35, 39, 48, 255 });

        DrawText("PROBABILITIES", 790, 165, 25, GOLD);

        DrawText("RED      : 48.65%", 790, 220, 22, RED);
        DrawText("BLACK : 48.65%", 790, 260, 22, LIGHTGRAY);
        DrawText("GREEN : 2.70%", 790, 300, 22, GREEN);

        DrawLine(790, 345, 1200, 345, GRAY);

        DrawText("POSSIBLE GAINS", 790, 370, 25, GOLD);

        DrawText(TextFormat("Red win: +%d", bet.amount),
                 790, 420, 20, RED);

        DrawText(TextFormat("Black win: +%d", bet.amount),
                 790, 455, 20, LIGHTGRAY);

        DrawText(TextFormat("Green win: +%d", bet.amount * 35),
                 790, 490, 20, GREEN);

        DrawText(TextFormat("Loss: -%d", bet.amount),
                 790, 530, 20, WHITE);

        // Game Over
        if (player.money <= 0)
        {
            DrawRectangle(
                0, 0,
                GetScreenWidth(),
                GetScreenHeight(),
                Color{ 0, 0, 0, 220 }
            );

            const char* gameOverText = "GAME OVER";
            int gameOverWidth = MeasureText(gameOverText, 50);

            DrawText(
                gameOverText,
                GetScreenWidth() / 2 - gameOverWidth / 2,
                200,
                50,
                RED
            );

            const char* lostText = "You have lost all your money!";
            int lostWidth = MeasureText(lostText, 25);

            DrawText(
                lostText,
                GetScreenWidth() / 2 - lostWidth / 2,
                280,
                25,
                WHITE
            );

            DrawText(
                TextFormat("Total gains: +%d", player.totalGains),
                GetScreenWidth() / 2 - 110,
                340,
                20,
                GREEN
            );

            DrawText(
                TextFormat("Total losses: -%d", player.totalLosses),
                GetScreenWidth() / 2 - 110,
                370,
                20,
                RED
            );

            DrawText(
                "Press R to play again",
                GetScreenWidth() / 2 - 130,
                440,
                25,
                GREEN
            );

            DrawText(
                "Press ESCAPE to quit",
                GetScreenWidth() / 2 - 125,
                490,
                25,
                LIGHTGRAY
            );
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
        return {};
    }

    bool IsRenderer() const override
    {
        return true;
    }
};
