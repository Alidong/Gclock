file(GLOB_RECURSE SOURCES 
"infra.c"
${CMAKE_CURRENT_SOURCE_DIR}/power/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
    device 
    vfs
)