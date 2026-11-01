//
// Created by archivist on 9/26/26.
//

#ifndef REDMAGIC_RENDERINGSYSTEM_HPP
#define REDMAGIC_RENDERINGSYSTEM_HPP
#include "ECS/ECS_Entity.hpp"
#include "ECS/ECS_System.hpp"
#include "ECS/Components/Camera.hpp"

namespace RedMagic::ECS {
    class Renderer2D final : virtual public System {
        ComponentRef<CameraComponent> camera;
        std::optional<RenderTexture> targetRenderTexture;
    public:
        explicit Renderer2D(const ECS::ComponentRef<CameraComponent> _camera)
        : camera(_camera)
        , targetRenderTexture(std::nullopt)
        {}
        explicit Renderer2D(const ECS::ComponentRef<CameraComponent> _camera, RenderTexture _targetRenderTexture)
        : camera(_camera)
        , targetRenderTexture(_targetRenderTexture)
        {}
        void Update(std::chrono::duration<double> delta_time) override;
        [[nodiscard]] std::vector<uint32_t> ReadComponentIDs() const override;
        [[nodiscard]] std::vector<uint32_t> WriteComponentIDs() const override;
        [[nodiscard]] bool IsRenderer() const override { return true; }
        ~Renderer2D() override = default;
    };
}

#endif //REDMAGIC_RENDERINGSYSTEM_HPP
