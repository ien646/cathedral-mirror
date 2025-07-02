set(TARGET_NAME cathedral-standalone)

add_executable(${TARGET_NAME}
        "src/main.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
)

target_link_libraries(${TARGET_NAME} PRIVATE
        libien

        cathedral-core
        cathedral-gfx
        cathedral-project
        cathedral-script
        cathedral-sdl
)

target_include_directories(${TARGET_NAME} PRIVATE "include")