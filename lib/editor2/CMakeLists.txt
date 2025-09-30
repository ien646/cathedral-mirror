set(TARGET_NAME cathedral-editor2)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp)

add_library(${TARGET_NAME} ${TARGET_SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_SCAN_FOR_MODULES OFF
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
        cathedral-engine
        cathedral-project
        cathedral-script
        cathedral-sdl
        imgui
        nfd
        SDL3::Headers
)