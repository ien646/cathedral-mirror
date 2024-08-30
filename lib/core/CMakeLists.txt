set(TARGET_NAME cathedral-core)

add_library(${TARGET_NAME}
    "src/error.cpp"
    "src/uid.cpp"
)

target_include_directories(${TARGET_NAME} PUBLIC include)