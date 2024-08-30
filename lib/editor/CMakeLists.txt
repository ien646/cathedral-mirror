set(TARGET_NAME cathedral-editor)

add_library(${TARGET_NAME}
    "include/cathedral/editor/editor_window_menubar.hpp"
    "include/cathedral/editor/editor_window.hpp"
    "include/cathedral/editor/properties_dock_widget.hpp"
    "include/cathedral/editor/scene_dock_widget.hpp"
    "include/cathedral/editor/scene_tree.hpp"
    "include/cathedral/editor/vulkan_widget.hpp"

    "include/cathedral/editor/common/code_editor.hpp"
    "include/cathedral/editor/common/dock_title.hpp"
    "include/cathedral/editor/common/path_selector.hpp"
    "include/cathedral/editor/common/slider.hpp"
    "include/cathedral/editor/common/sliding_float.hpp"
    "include/cathedral/editor/common/text_input_dialog.hpp"
    "include/cathedral/editor/common/transform_widget.hpp"
    "include/cathedral/editor/common/vector3_widget.hpp"
    "include/cathedral/editor/common/vertical_separator.hpp"

    "include/cathedral/editor/node_properties/camera3d_properties_widget.hpp"
    "include/cathedral/editor/node_properties/mesh3d_properties_widget.hpp"

    "include/cathedral/editor/asset_managers/shader_manager.hpp"

    "src/editor_window_menubar.cpp"
    "src/editor_window.cpp"
    "src/properties_dock_widget.cpp"
    "src/scene_dock_widget.cpp"
    "src/scene_tree.cpp"
    "src/styling.cpp"
    "src/vulkan_widget.cpp"

    "src/common/code_editor.cpp"
    "src/common/dock_title.cpp"
    "src/common/message.cpp"
    "src/common/path_selector.cpp"
    "src/common/slider.cpp"
    "src/common/sliding_float.cpp"
    "src/common/text_input_dialog.cpp"
    "src/common/transform_widget.cpp"
    "src/common/vector3_widget.cpp"
    "src/common/vertical_separator.cpp"
    
    "src/node_properties/camera3d_properties_widget.cpp"
    "src/node_properties/mesh3d_properties_widget.cpp"

    "src/asset_managers/shader_manager.cpp"
    "src/asset_managers/shader_manager.ui"
    "src/asset_managers/shader_syntax_highlighter.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
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
    cathedral-project
)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_precompile_headers(${TARGET_NAME} PRIVATE 
    <QDockWidget>
    <QFont>
    <QLabel>
    <QHBoxLayout>
    <QMainWindow>
    <QMenuBar>
    <QMessageBox>
    <QString>
    <QTimer>
    <QTreeWidget>
    <QVBoxLayout>
    <QVulkanInstance>
    <QWidget>
    <QWindow>
)