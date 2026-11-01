add_executable(CasinoECS
    main.cpp
        CasinoComponents.cpp
        Systems/RouletteSystem.hpp
        Systems/CasinoRenderSystem.hpp
        Systems/CasinoInputSystem.hpp
        Utils/RouletteUtils.hpp
)

target_link_libraries(CasinoECS PUBLIC RedMagic)
