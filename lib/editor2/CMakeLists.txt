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

if (UNIX)
    if (CATHEDRAL_LINUX_FORCE_X11)
        target_compile_definitions(${TARGET_NAME} PUBLIC CATHEDRAL_LINUX_PLATFORM_X11)
        target_link_libraries(${TARGET_NAME} PUBLIC X11)
    else()
        target_compile_definitions(${TARGET_NAME} PUBLIC CATHEDRAL_LINUX_PLATFORM_WAYLAND)
        target_link_libraries(${TARGET_NAME} PUBLIC WaylandQtPointerConstraints)
    endif ()
endif ()