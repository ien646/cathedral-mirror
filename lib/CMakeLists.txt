set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(CLANG_TIDY_PROGRAM clang-tidy)
if(CLANG_TIDY_PROGRAM)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy "-checks=\
        bugprone-*,\
        clang-diagnostic-*,\
        clang-analyzer-*,\
        modernize-*,\
        performance-*,\
        readability-*,\
        misc-unused-parameters,\
        cppcoreguidelines-pro-type-cstyle-cast,
        openmp-*, \
        portability-*, \
    
        -modernize-use-trailing-return-type, \
        -modernize-use-nodiscard, \
        -modernize-use-std-numbers, \
        -modernize-use-transparent-functors, \
        -modernize-use-bool-literals,\
        -readability-identifier-length,\
        -readability-magic-numbers"
    )
endif()

if(MSVC)
    add_compile_options("/W4")
else()
    add_compile_options("-Wall" "-Wpedantic")
endif()

add_subdirectory(core)
add_subdirectory(editor)
add_subdirectory(engine)
add_subdirectory(gfx)
add_subdirectory(project)