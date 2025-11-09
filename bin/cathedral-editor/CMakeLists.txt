set(TARGET_NAME cathedral-app-editor)

add_executable(${TARGET_NAME}
        "src/main.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_link_libraries(${TARGET_NAME} PRIVATE
        libien

        cathedral-core
        cathedral-editor
        cathedral-gfx
        cathedral-project
        cathedral-script
)

target_include_directories(${TARGET_NAME} PRIVATE "include")

if (CATHEDRAL_APP_editor_INITIAL_PROJECT_DIR)
    target_compile_definitions(${TARGET_NAME} PUBLIC CATHEDRAL_APP_editor_INITIAL_PROJECT_DIR="${CATHEDRAL_APP_editor_INITIAL_PROJECT_DIR}")
endif ()