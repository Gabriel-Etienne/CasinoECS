# Loads the FetchContent module, it only needs to be done once
include(FetchContent)
include(ExternalProject)

# Declares a resource to fetch
FetchContent_Declare(
        RayLib
        GIT_REPOSITORY https://github.com/raysan5/raylib
        GIT_TAG 6.0
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(RayLib)

add_library(RedMagic STATIC
        insertion_sort.hpp
        indexed_vector.hpp
        ECS/ECS_Entity.cpp
        ECS/ECS_Entity.hpp
        ECS/ECS_System.hpp
        ECS/Components/Transform2D.hpp
        ECS/Components/Camera.hpp
        ECS/Components/Sprite2D.hpp
        ECS/Systems/RenderingSystem.hpp
        ECS/Systems/RenderingSystem.cpp
        ECS/ECS_ComponentsRepository.cpp
        ECS/ECS_System.cpp
)

target_include_directories(RedMagic PUBLIC .)
target_link_libraries(RedMagic PUBLIC raylib)