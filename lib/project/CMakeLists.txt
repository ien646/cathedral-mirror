set(TARGET_NAME cathedral-project)

add_library(${TARGET_NAME}
    "src/asset.cpp"
    "src/project.cpp"

    "src/assets/shader_asset.cpp"
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
    cathedral-core
    cathedral-gfx
    cathedral-engine

    nlohmann_json
)