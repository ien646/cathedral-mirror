set(TARGET_NAME cathedral-editor2)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp)

CPMAddPackage(
        NAME imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.92.3-docking
        SYSTEM ON
        DOWNLOAD_ONLY ON
)
if (imgui_ADDED)
    set(IMGUI_SOURCES
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
            ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    )
    add_library(imgui ${IMGUI_SOURCES})
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
    target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL3-static)
endif ()

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

if (CATHEDRAL_PCH)
    target_precompile_headers(${TARGET_NAME} PUBLIC
            <cathedral/core.hpp>

            <algorithm>
            <array>
            <filesystem>
            <functional>
            <memory>
            <optional>
            <ranges>
            <string>
            <vector>

            <imgui.h>
            <imgui_internal.h>

            <magic_enum.hpp>
    )
endif ()