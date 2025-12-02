if(CATHEDRAL_LINUX_FORCE_X11)
    add_compile_definitions(CATHEDRAL_LINUX_PLATFORM_X11)
endif()

add_subdirectory(cathedral-editor)
add_subdirectory(cathedral-standalone)