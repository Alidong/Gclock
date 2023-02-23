file(GLOB_RECURSE SOURCES 
"server.c"
${CMAKE_CURRENT_SOURCE_DIR}/http/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
    device
    esp-tls
    esp_http_server
    json
)