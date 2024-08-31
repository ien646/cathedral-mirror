set(TARGET_NAME cathedral-engine)

add_library(${TARGET_NAME}
    "src/camera.cpp"
    "src/material.cpp"
    "src/mesh.cpp"
    "src/mesh_buffer_storage.cpp"
    "src/scene.cpp"
    "src/scene_node.cpp"
    "src/renderer.cpp"
    "src/texture.cpp"
    "src/transform.cpp"
    "src/upload_queue.cpp"
    "src/vertex_input_builder.cpp"

    "src/default_resources/default_texture.cpp"

    "src/nodes/camera3d_node.cpp"
    "src/nodes/mesh3d_node.cpp"
    "src/nodes/node.cpp"
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
    cathedral-core
    cathedral-gfx
    glm
    happly
)