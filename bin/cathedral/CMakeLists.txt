set(TARGET_NAME cathedral)

qt_standard_project_setup()

qt_add_executable(${TARGET_NAME}
    "src/main.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_link_libraries(${TARGET_NAME} PRIVATE
    libien
    Qt6::Core
    Qt6::Widgets

    cathedral-core
    cathedral-editor
    cathedral-gfx
)

target_include_directories(${TARGET_NAME} PRIVATE "include")

add_custom_target(
	${TARGET_NAME}_rsc
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/rsc ${CMAKE_CURRENT_BINARY_DIR}/rsc
)

add_dependencies(${TARGET_NAME} ${TARGET_NAME}_rsc)

qt_generate_deploy_app_script(
    OUTPUT_SCRIPT DEPLOY_SCRIPT
    TARGET ${TARGET_NAME}
    NO_TRANSLATIONS
)

install(SCRIPT ${DEPLOY_SCRIPT})

if(WIN32)
    find_program(WINDEPLOYQT NAMES windeployqt.exe HINTS ${CMAKE_PREFIX_PATH} REQUIRED)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${WINDEPLOYQT} $<TARGET_FILE:${TARGET_NAME}>
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${TARGET_NAME}>
    )
endif()