set(TARGET_NAME cathedral-editor)

file(GLOB_RECURSE TARGET_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include *.hpp)
file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp)
file(GLOB_RECURSE TARGET_UI_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src *.ui)
file(GLOB_RECURSE TARGET_RCC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rsc *.qrc)

add_library(${TARGET_NAME}
    ${TARGET_HEADERS}
    ${TARGET_SOURCES}
    ${TARGET_UI_FILES}
    ${TARGET_RCC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt6::Concurrent
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    
    cathedral-core
    cathedral-engine
    cathedral-gfx
    cathedral-project
)

target_include_directories(${TARGET_NAME} PUBLIC include)