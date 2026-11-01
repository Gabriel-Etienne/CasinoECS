//
// Created by archivist on 9/26/26.
//

#ifndef REDMAGIC_TRANSFORM2D_HPP
#define REDMAGIC_TRANSFORM2D_HPP

#include "raylib.h"
#include "raymath.h"
#include "indexed_vector.hpp"
#include "ECS/ECS_Entity.hpp"

namespace RedMagic::ECS {
    struct Transform2D {
        Vector2 Position;
        float   Rotation;
        Vector2 Scale;

        static constexpr Transform2D Identity() {
            return {
                .Position = {.x = 0, .y = 0},
                .Rotation = 0.0f,
                .Scale    = {.x = 1, .y = 1},
            };
        }

        void Rotate(const float angle) {
            Rotation += angle;
        }

        [[nodiscard]] Transform ToRaylib() const {
            return {
                .translation = {.x = Position.x, .y = 0.0f, .z = Position.y},
                .rotation    = QuaternionFromAxisAngle({.x = 0.0f, .y = 1.0f, .z = 0.0f}, Rotation),
                .scale       = {.x = Scale.x, .y = 1.0f, .z = Scale.y},
            };
        }
    };

    static_assert(std::is_trivially_default_constructible_v<Transform2D>);

    /**
     * This class represents a transform (combination of translation, rotation
     * and scaling) for an entity in the ECS. The transform component detects
     * when the entity has a parent and will provide a WorldTransform that
     * corresponds to where the current entity is placed globally, and contain
     * internally offsets to the parent.
     */
    struct Transform2DComponent {
    private:
        enum class ParentStatus {
            uninitialized,
            no_parent,
            parent_found
        };

        /**
         * Contains the status of the component relating whether a parent for
         * it has been set and whether that parent has been determined. This
         * avoids recomputation of the lookup of a parent's transform every
         * frame for 2D objects that need to be displayed
         */
        mutable ParentStatus status = ParentStatus::uninitialized;
        /**
         * The stored parent if it has been cached and found.
         */
        mutable ComponentRef<Transform2DComponent> parentComponent{};
    public:
        /**
         * This class needs to be notified if entity parenting status changes.
         */
        static constexpr bool notify_reparent = true;
        /**
         * Every component with `notify_reparent` set to true needs this.
         */
        void NotifyReparent() {status = ParentStatus::uninitialized;}
        /**
         * Every component needs this
         */
        static constexpr uint32_t component_type = 0;
        /**
         * Every component needs this
         */
        static indexed_vector<Transform2DComponent> storage;
        /**
         * Every component needs this
         */
        Entity owner;
        /**
         * The implementation of the transform.
         */
        Transform2D Transform = Transform2D::Identity();

        /**
         * Obtains the absolute value of the transform of the current object.
         * This may involve re-resolving its parent(s) but is otherwise just
         * some math away.
         * @return The solved absolute transform.
         */
        [[nodiscard]] Transform2D WorldTransform() const {
            Transform2D tr [[indeterminate]];
            if (status == ParentStatus::uninitialized) [[unlikely]] {
                auto trMode = owner
                    .ParentId()
                    .and_then(GetTransformComponent);
                if (trMode) {
                    status = ParentStatus::parent_found;
                    parentComponent = trMode.value();
                } else {
                    status = ParentStatus::no_parent;
                }
            }
            switch (status) {
                case ParentStatus::no_parent:
                    return Transform;
                case ParentStatus::parent_found: {
                    auto [originPosition, originRotation, originScale] = parentComponent->WorldTransform();
                    const Transform2D result = {
                        .Position = originPosition + originScale * Vector2Rotate(Transform.Position, originRotation),
                        .Rotation = Transform.Rotation + originRotation,
                        .Scale    = Transform.Scale * originScale
                    };
                    return result;
                }
                default: std::unreachable();
            }
        }
    private:
        /**
         * From entity, get its Transform2DComponent
         */
        constexpr static auto GetTransformComponent =
            [](const auto& entity){
                return entity.template GetComponent<Transform2DComponent>();
        };
    };
}
#endif //REDMAGIC_TRANSFORM2D_HPP
