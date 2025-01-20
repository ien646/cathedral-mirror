set(TARGET_NAME cathedral-project)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src "*.cpp")
file(GLOB_RECURSE TARGET_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src "*.hpp")

add_library(${TARGET_NAME}
   ${TARGET_SOURCES}
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
    cathedral-core
    cathedral-gfx
    cathedral-engine
)