#include "Components/PlayerComponent.hpp"
#include "Components/BetComponent.hpp"
#include "Components/RouletteComponent.hpp"

RedMagic::indexed_vector<PlayerComponent> PlayerComponent::storage;
RedMagic::indexed_vector<BetComponent> BetComponent::storage;
RedMagic::indexed_vector<RouletteComponent> RouletteComponent::storage;