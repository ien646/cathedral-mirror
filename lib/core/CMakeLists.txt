set(TARGET_NAME cathedral-core)

add_library(${TARGET_NAME}
        "src/compression.cpp"
        "src/error.cpp"
        "src/log.cpp"

        "src/bits/alloc.cpp"
        "src/bits/scratch_memory.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC cereal libien lz4 magic_enum icecream-cpp)