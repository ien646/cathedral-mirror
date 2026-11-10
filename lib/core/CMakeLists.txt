set(TARGET_NAME cathedral-core)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp)

add_library(${TARGET_NAME} ${TARGET_SOURCES})

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
        cereal::cereal
        glm::glm
        libien::libien
        lz4
        magic_enum
        icecream-cpp
        Boost::regex
        Boost::preprocessor
        Boost::unordered
)