#include "ECS_Entity.hpp"

namespace RedMagic::ECS {
    std::optional<Entity> Entity::ParentId() const {
        assert(Shadow::Entity::storage.has_element(EntityId) && "Cannot find parent of an entity that doesn't exist");
        return Shadow::Entity::storage[EntityId].ParentId;
    }

    Entity Entity::CreateChild() const {
        assert(Shadow::Entity::storage.has_element(EntityId) && "Cannot insert entity, parent does not exist");
        const Entity entity = { Shadow::Entity::storage.emplace_back() };
        auto& entity_ref = Shadow::Entity::storage[entity.EntityId];
        entity_ref.ParentId = *this;
        entity_ref.EntityId = entity;
        assert(Shadow::Entity::storage.has_element(entity.EntityId) && "");
        return entity;
    }

    Entity Entity::CreateEntity() {
        assert(not Shadow::Entity::storage.full() && "Cannot insert entity, container full");
        const Entity entity = { Shadow::Entity::storage.emplace_back() };
        Shadow::Entity::storage[entity.EntityId].EntityId = entity;
        Shadow::Entity::storage[entity.EntityId].ParentId = std::nullopt;
        assert(Shadow::Entity::storage.has_element(entity.EntityId) && "Entity just created but not found in container");
        return entity;
    }

    size_t Entity::Count() {
        return Shadow::Entity::storage.size();
    }

    void Entity::DestroyEntity() {
        assert(EntityId != -1 && "Destroying an uninitialized entity or destroyed entity, did you call this twice in a row?");
        assert(not Shadow::Entity::storage.empty() && "Destroying but no entity exists at this time");
        assert(Shadow::Entity::storage.has_element(EntityId) && "Destroying an entity that doesn't exist");
        // Mark the data for deletion
        Shadow::Entity::storage.mark_for_discard(EntityId);
        Shadow::Entity::destroyed.push_back(*this);
        EntityId = -1;
    }

    void Entity::CollectAllChildren() {
        auto& table = Shadow::Entity::destroyed;
        // Keep clearing until no entities to clear are left
        while (not table.empty()) {
            // Mark how many we will clear this pass
            size_t phase_targets = table.size();
            for (auto& entity : Shadow::Entity::storage) {
                if (not entity.ParentId) continue;
                const auto& v = entity.ParentId.value();
                if (std::ranges::find(table, v.EntityId, [](auto e){return e.EntityId;}) != table.end()) {
                    entity.EntityId.DestroyEntity();
                }
                // Clear all the components and destroy them (we should defer that)
                for (auto [idx, type] : entity.Components) {
                    Shadow::Entity::destructors[type](idx);
                }
                entity.Components.clear();
            }
            // mark the clearing of that pass as done
            while (phase_targets) {
                assert(phase_targets > 0 and not table.empty() && "Can't remove things when the number of things is 0");
                table.pop_front();
                phase_targets--;
            }
        }
        // We can now actually clear everything in the tree at once
        Shadow::Entity::storage.collect_all_marks();
        for (const auto& collector: Shadow::Entity::collectors | std::views::values) {
            collector();
        }
    }
}