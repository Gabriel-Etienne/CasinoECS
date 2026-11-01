//
// Created by archivist on 9/26/26.
//

#ifndef REDMAGIC_SPRITE2D_HPP
#define REDMAGIC_SPRITE2D_HPP
#include <cstdint>
#include <memory>
#include <utility>

#include "raylib.h"
#include "indexed_vector.hpp"
#include "Transform2D.hpp"
#include "ECS/ECS_Entity.hpp"

namespace RedMagic::ECS {
    struct SpriteComponent {
        /**
         * Every component needs this
         */
        static constexpr bool notify_reparent = true;
        /**
         * Every component needs this
         */
        static constexpr uint32_t component_type = 2;
        /**
         * Every component needs this
         */
        static indexed_vector<SpriteComponent> storage;
        /**
         * Every component needs this
         */
        Entity owner;
        /**
         * Every component with `notify_reparent` set to true needs this
         */
        void NotifyReparent() {transform = std::nullopt;}

        /**
         * This is a cached transform reference.
         * @note Please do not set or read from this directly, go through the `GetTransform2D`.
         */
        mutable std::optional<ComponentRef<Transform2DComponent>> transform;
        /**
         * Cache handling to fetch the transform above.
         * @return The reference, will crash if the reference cannot be found anywhere when walking up the scene graph.
         */
        ComponentRef<Transform2DComponent> GetTransform2D() const {
            std::optional e = owner;
            while (e and not transform) [[unlikely]] {
                transform = e->GetComponent<Transform2DComponent>();
                e = e->ParentId();
            }
            return transform.value();
        }

        std::shared_ptr<Texture2D> texture;
        Vector2 offset;
        Rectangle srcRect;
        float z_order = 0.0f;

        /**
         * @param _texture The texture where the sprite is.
         * @param _offset The center of the displayed sprite for purpose of positioning, rotation, scaling.
         * @param _srcRect The sample of the texture that is displayed, negative width and height can be used to flip
         * the texture vertically or horizontally.
         */
        explicit SpriteComponent(
            std::shared_ptr<Texture2D> _texture,
            const Vector2 _offset = {.x = 0, .y = 0},
            const Rectangle _srcRect = {.x = -1, .y = -1, .width = -1, .height = -1}
        )
        : texture(std::move(_texture))
        , offset(_offset)
        , srcRect(_srcRect) {
            // if the source is the default value, we make it the full sprite
            if (
                srcRect.x == -1
                and srcRect.y == -1
                and srcRect.width == -1
                and srcRect.height == -1
            ) {
                srcRect = {
                    .x = 0.0f,
                    .y = 0.0f,
                    .width = static_cast<float>(texture->width),
                    .height = static_cast<float>(texture->height)
                };
            }
        }
    };
}

#endif //REDMAGIC_SPRITE2D_HPP
