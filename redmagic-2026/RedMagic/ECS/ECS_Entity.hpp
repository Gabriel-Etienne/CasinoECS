#pragma once
#include <cassert>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <optional>

#include "indexed_vector.hpp"

namespace RedMagic::ECS {
    /**
     * A reference to a component
     * @tparam T The type
     */
    template<typename T>
    struct ComponentRef {
        uint32_t index = -1;
        [[nodiscard]] T& operator*() noexcept {
            assert(not T::storage.empty() && "Component does not exist, none of these actually exist");
            assert(T::storage.has_element(index) && "Component does not exist");
            return T::storage[index];
        }
        [[nodiscard]] T* operator->() noexcept {
            assert(not T::storage.empty() && "Component does not exist, none of these actually exist");
            assert(T::storage.has_element(index) && "Component does not exist");
            return std::addressof(T::storage[index]);
        }
        [[nodiscard]] const T& operator*() const noexcept {
            assert(not T::storage.empty() && "Component does not exist, none of these actually exist");
            assert(T::storage.has_element(index) && "Component does not exist");
            return T::storage[index];
        }
        [[nodiscard]] const T* operator->() const noexcept {
            assert(not T::storage.empty() && "Component does not exist, none of these actually exist");
            assert(T::storage.has_element(index) && "Component does not exist");
            return std::addressof(T::storage[index]);
        }
    };

    struct Entity {
        uint64_t EntityId;

        /**
         * Gets a component by type
         * @tparam T The type
         * @return Maybe a reference to a component
         */
        template<typename T>
        [[nodiscard]] std::optional<ComponentRef<T>> GetComponent() const;

        /**
         * Creates a new component
         * @tparam T  The type
         * @param args The arguments for the constructor of the argument
         * @return A reference to the component
         */
        template<typename T>
        ComponentRef<T> CreateComponent(auto&&... args) const;

        /**
         * Gets the parent of a component
         * @return Maybe the parent if it exists
         */
        [[nodiscard]] std::optional<Entity> ParentId() const ;

        /**
         * Creates a child entity to the current entity
         * @return the child entity
         */
        [[nodiscard]] Entity CreateChild() const;

        /**
         * Destroys the entity
         * @note It doesn't destroy the entity, the systems will collect all entities marked for destruction at the end
         * of the frame.
         * @bug It does, however, immediately delete the components, this should be deferred as well
         */
        void DestroyEntity();

        /**
         * Destroys all entities marked for destruction and their children and components
         * @internal
         * This is used internally by System::UpdateAll
         * @endinternal
         */
        static void CollectAllChildren();

        /**
         * Creates a new entity
         * @return The new entity
         */
        static Entity CreateEntity();

        /**
         * @return the number of entities in existence
         */
        static size_t Count();
    };

    /**
     * How entities represent their components internally, you should never have to manipulate this by hand
     */
    struct Component {
        uint32_t ComponentId;
        uint32_t ComponentType;
    };

    namespace Shadow {
        /**
         * Entity bookkeeping internals
         */
        struct Entity {
            static std::map<uint32_t, std::function<void(uint32_t)>> destructors;
            static std::map<uint32_t, std::function<void()>>         collectors;
            static indexed_vector<Entity> storage;
            static std::deque<ECS::Entity> destroyed;
            std::optional<ECS::Entity> ParentId;
            ECS::Entity EntityId;
            std::vector<Component> Components;
        };
    }

    template<typename T>
    std::optional<ComponentRef<T>> Entity::GetComponent() const {
        assert(Shadow::Entity::storage.has_element(EntityId) && "Entity does not exist");
        ComponentRef<T> result;
        for (auto[ComponentId, ComponentType] : Shadow::Entity::storage[EntityId].Components) {
            if (ComponentType == T::component_type) {
                assert(T::storage.has_element(ComponentId) && "Component does not exist but is in the entity anyway");
                result.index = ComponentId;
                return result;
            }
        }
        return std::nullopt;
    }

    template<typename T>
    ComponentRef<T> Entity::CreateComponent(auto&&... args) const {
        assert(Shadow::Entity::storage.has_element(EntityId) && "Entity does not exist");
        {
            auto it = Shadow::Entity::destructors.find(T::component_type);
            if (it == Shadow::Entity::destructors.end()) {
                Shadow::Entity::destructors.insert({T::component_type, [](uint32_t id) noexcept { T::storage.mark_for_discard(id); }});
            }
        }
        {
            auto it = Shadow::Entity::collectors.find(T::component_type);
            if (it == Shadow::Entity::collectors.end()) {
                Shadow::Entity::collectors.insert({T::component_type, []() noexcept { T::storage.collect_all_marks(); }});
            }
        }
        ComponentRef<T> ret;
        assert(not T::storage.full() && "Could not insert component in full container");
        auto ref{ T::storage.emplace_back(std::forward<decltype(args)>(args)...) };
        assert(not Shadow::Entity::storage.full() && "Entity storage is full which is abnormal");
        Shadow::Entity::storage[EntityId].Components.push_back({
            .ComponentId = ref,
            .ComponentType = T::component_type,
        });
        ret = {ref};
        ret->owner = {EntityId};
        return ret;
    }

    // Do not use this component, it is for demonstration only
    // Components should look just like this one and all have different `component_type`s.
    struct ArchetypalComponent {
        static constexpr uint32_t component_type = 0;
        static constexpr bool notify_reparent = false;
        static indexed_vector<ArchetypalComponent> storage;
        Entity owner;
    };
}
