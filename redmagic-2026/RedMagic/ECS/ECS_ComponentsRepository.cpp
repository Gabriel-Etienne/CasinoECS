#include "ECS_Entity.hpp"

#include "Components/Camera.hpp"
#include "Components/Sprite2D.hpp"
#include "Components/Transform2D.hpp"

#include "ECS_System.hpp"

using namespace RedMagic;

// Components
indexed_vector<ECS::Transform2DComponent>   ECS::Transform2DComponent::storage;
indexed_vector<ECS::CameraComponent>        ECS::CameraComponent::storage;
indexed_vector<ECS::SpriteComponent>        ECS::SpriteComponent::storage;

// Entities
indexed_vector<ECS::Shadow::Entity>               ECS::Shadow::Entity::storage;
std::map<uint32_t, std::function<void(uint32_t)>> ECS::Shadow::Entity::destructors;
std::map<uint32_t, std::function<void()>>         ECS::Shadow::Entity::collectors;
std::deque<ECS::Entity>                           ECS::Shadow::Entity::destroyed;

// Systems
indexed_vector<std::shared_ptr<ECS::System>, uint16_t> ECS::System::storage;