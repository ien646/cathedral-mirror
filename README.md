Cathedral. A very (very) WIP engine and editor
-----
<img width="2654" height="2074" alt="Screenshot_20251020_220655" src="https://github.com/user-attachments/assets/2d8193ee-1fbd-45d3-a39b-b41b694eb1c7" />

### Build requirements

---

- **CMake** >= 3.28
- **C++ compiler**:
  - **Clang** 19 or greater. If in doubt, use this.
  - **GCC** 14 or greater, ocassionally tested.
  - **Visual Studio** 2022, builds might not work at times due to differences with clang.
- **Vulkan SDK** with support for Core >= 1.3
    - On Windows, shaderc-combined debug libraries must be installed
    - On development builds, validation layers must be installed.

### Running requirements

---
Check your device here: https://vulkan.gpuinfo.org/
- Vulkan physical device features:

  - Core 1.0:
    - Sampler anisotropy
    - Fill mode non-solid
    - Robust buffer access
    - Sample rate shading
  - Core 1.3:
    - Dynamic rendering
    - Syncronization 2

- Vulkan extensions:
  - VK_EXT_memory_budget
    

### Environment

---

- `VULKAN_SDK` pointing to VulkanSDK directory

### Flags

---

- `#define CATHEDRAL_NO_CHECKS` to disable runtime checks
- `#define CATHEDRAL_NO_TRACING` to disable source code information in error traces

### CMake options

---

- `CATHEDRAL_SANITIZE_ADDRESS` -> enable ASAN
- `CATHEDRAL_BUILD_TESTS` -> build tests
- `CATHEDRAL_PCH` -> enable precompiled headers during build
- `CATHEDRAL_LINUX_FORCE_X11` -> force X11 build on Linux
- `CATHEDRAL_APP_editor_INITIAL_PROJECT_DIR` -> setting this option to an existing project directory will open it automatically when running a dev build of the editor