set(TARGET_NAME cathedral-core)

add_library(${TARGET_NAME}
    "src/error.cpp"
    "src/uid.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${TARGET_NAME} PUBLIC include)