set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(
            /W4
            /wd4702 #unreachable warnings on [[noreturn]] calls
    )
else()
    add_compile_options(
            -Wall
            -Wpedantic
    )
endif()

if(CATHEDRAL_NO_CHECKS)
    add_compile_definitions(CATHEDRAL_NO_CHECKS)
endif()

add_subdirectory(core)
add_subdirectory(editor)
add_subdirectory(engine)
add_subdirectory(gfx)
add_subdirectory(project)
add_subdirectory(script)
add_subdirectory(sdl)