set(TARGET_NAME cathedral-editor2)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp)

add_library(${TARGET_NAME} ${TARGET_SOURCES})

target_link_libraries(${TARGET_NAME} PUBLIC
        cathedral-core
        cathedral-engine
        cathedral-gfx
        cathedral-project
        cathedral-script
        cathedral-sdl
        cathedral-resources
        imgui
        nfd
)

target_include_directories(${TARGET_NAME} PUBLIC include)

if (UNIX)
    if (CATHEDRAL_LINUX_FORCE_X11)
        target_compile_definitions(${TARGET_NAME} PUBLIC CATHEDRAL_LINUX_PLATFORM_X11)
    else ()
    endif ()
endif ()