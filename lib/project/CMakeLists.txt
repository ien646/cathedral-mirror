set(TARGET_NAME cathedral-project)

add_library(${TARGET_NAME}
    "src/asset.cpp"
    "src/project.cpp"

    "src/assets/material_definition_asset.cpp"
    "src/assets/shader_asset.cpp"
    "src/assets/texture_asset.cpp"
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
    cathedral-core
    cathedral-gfx
    cathedral-engine

    magic_enum
    nlohmann_json
)