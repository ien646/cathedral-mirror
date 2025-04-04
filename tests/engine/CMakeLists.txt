set(PROJECT_NAME "cathedral-tests-engine")

add_executable(${PROJECT_NAME}
    shader_preprocess.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    cathedral-engine
    Catch2::Catch2WithMain
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)