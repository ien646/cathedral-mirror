set(TARGET_NAME cathedral-editor)

add_library(${TARGET_NAME}
    "include/cathedral/editor/editor_window_menubar.hpp"
    "src/editor_window_menubar.cpp"

    "include/cathedral/editor/editor_window.hpp"
    "src/editor_window.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt6::Core
    Qt6::Widgets
    
    cathedral-core
    cathedral-engine
    cathedral-gfx
)

target_include_directories(${TARGET_NAME} PUBLIC include)