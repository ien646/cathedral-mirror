Requirements
----------------------

- CMake >= 3.28
- GCC (>=14), Clang (>=19) or MSVC, with C++23 support
- VulkanSDK supporting Vulkan >= 1.3
    - On Windows, shaderc-combined debug libraries
- For editor:
    - Qt6 (Widgets, OpenGL, Concurrent)

Environment
----------------------

- VULKAN_SDK pointing to VulkanSDK directory
- On Windows, QT_DIR pointing to Qt6 directory

Flags
----------------------

- `#define CATHEDRAL_NO_CHECKS` to disable runtime checks
- `#define CATHEDRAL_NO_TRACING` to disable source code information in error traces