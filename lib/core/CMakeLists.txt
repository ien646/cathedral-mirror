set(TARGET_NAME cathedral-core)

add_library(${TARGET_NAME}
    "src/compression.cpp"
    "src/error.cpp"
    "src/uid.cpp"
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} libien lz4)