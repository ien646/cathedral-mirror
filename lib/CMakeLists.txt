set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#find_program(CLANG_TIDY_PROGRAM clang-tidy)
#if(CLANG_TIDY_PROGRAM)
#   if(MSVC)
#       set(CMAKE_CXX_CLANG_TIDY clang-tidy --extra-arg=/EHsc)
#   else()
#        set(CMAKE_CXX_CLANG_TIDY clang-tidy)
#    endif()
#endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wpedantic)
endif()

#add_compile_definitions(CATHEDRAL_NO_CHECKS)

add_subdirectory(core)
add_subdirectory(editor)
add_subdirectory(engine)
add_subdirectory(gfx)
add_subdirectory(project)
add_subdirectory(script)