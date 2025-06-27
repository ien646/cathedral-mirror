set(TARGET_NAME cathedral-script)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src "*.cpp")
file(GLOB_RECURSE TARGET_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include "*.hpp")

add_library(${TARGET_NAME}
        ${TARGET_SOURCES}
        ${TARGET_HEADERS}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_SCAN_FOR_MODULES OFF)

target_compile_definitions(${TARGET_NAME} PUBLIC SOL_PRINT_ERRORS)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC cathedral-engine lua sol2)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /bigobj)
endif()