set(TARGET_NAME cathedral-resources)

add_library(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

macro(embed RELPATH)
    b_embed(${TARGET_NAME} "${EMBED_PREFIX}/${RELPATH}")
endmacro(embed)

set(EMBED_PREFIX "editor")
include(editor/resources.cmake)

set(EMBED_PREFIX "engine")
include(engine/resources.cmake)