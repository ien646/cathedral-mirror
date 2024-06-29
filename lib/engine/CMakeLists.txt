set(TARGET_NAME cathedral-engine)

add_library(${TARGET_NAME}
    "src/camera.cpp"
    "src/renderer.cpp"
    "src/transform.cpp"
    "src/upload_queue.cpp"
    "src/vertex_input_builder.cpp"

    "src/materials/world_geometry.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
    cathedral-core
    cathedral-gfx
    glm
)