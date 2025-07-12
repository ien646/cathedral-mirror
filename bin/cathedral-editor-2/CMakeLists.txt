set(TARGET_NAME cathedral-app-editor2)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp)

add_executable(${TARGET_NAME} "src/main.cpp")

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
        cathedral-editor2
        cathedral-gfx
        cathedral-project
        cathedral-script
        cathedral-sdl
        imgui
)