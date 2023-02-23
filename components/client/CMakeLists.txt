file(GLOB_RECURSE SOURCES 
"client.c"
${CMAKE_CURRENT_SOURCE_DIR}/http_req/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
    device
    esp-tls
    lwip
    esp_event
)