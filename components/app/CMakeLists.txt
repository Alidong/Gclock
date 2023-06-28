file(GLOB_RECURSE SOURCES 
# ${CMAKE_CURRENT_SOURCE_DIR}/server/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/client/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/*.c
${CMAKE_CURRENT_SOURCE_DIR}/infra/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
    pal
    esp-tls
    esp_http_server
    json
    esp_event
    vfs
)