//
// Created by archivist on 9/26/26.
//


#include <chrono>
#include <cstdint>

#include "ECS/Systems/RenderingSystem.hpp"
#include "ECS/Components/Camera.hpp"
#include "ECS/Components/Sprite2D.hpp"
#include "ECS/Components/Transform2D.hpp"

void RedMagic::ECS::Renderer2D::Update(std::chrono::duration<double>) {
    camera->camera.target = camera->owner.GetComponent<Transform2DComponent>().value()->WorldTransform().Position;
    BeginMode2D(camera->camera);
    if (targetRenderTexture) {
        BeginTextureMode(*targetRenderTexture);
    }
    ClearBackground(SKYBLUE);

    // Sort by z-order
    ECS::SpriteComponent::storage.sort(
        std::less{},
        [](const ECS::SpriteComponent& comp) noexcept -> float { return comp.z_order; }
    );

    // Draw all the sprites
    for (const auto& elem : ECS::SpriteComponent::storage) {
        const auto& tr = elem.GetTransform2D();
        const auto& [Position, Rotation, Scale] = tr->WorldTransform();
        const auto frame_offset = elem.offset * Scale;
        auto destRect = elem.srcRect;
        destRect.x = Position.x;
        destRect.y = Position.y;
        destRect.width *= Scale.x;
        destRect.height *= Scale.y;
        DrawTexturePro(*elem.texture, elem.srcRect, destRect, frame_offset, Rotation, WHITE);
    }
    if (targetRenderTexture) {
        EndTextureMode();
    }
    EndMode2D();
}
std::vector<uint32_t> RedMagic::ECS::Renderer2D::ReadComponentIDs() const {
    return {ECS::Transform2DComponent::component_type, ECS::CameraComponent::component_type, ECS::SpriteComponent::component_type};
}
std::vector<uint32_t> RedMagic::ECS::Renderer2D::WriteComponentIDs() const {
    return {};
}
