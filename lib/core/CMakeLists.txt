set(TARGET_NAME cathedral-core)

add_library(${TARGET_NAME}
    "src/compression.cpp"
    "src/error.cpp"
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC cereal libien lz4 magic_enum nlohmann_json icecream-cpp)