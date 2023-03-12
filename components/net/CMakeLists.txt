file(GLOB_RECURSE SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/server/*.c
${CMAKE_CURRENT_SOURCE_DIR}/server/http_server/*.c
${CMAKE_CURRENT_SOURCE_DIR}/client/*.c
${CMAKE_CURRENT_SOURCE_DIR}/client/http_client/*.c
${CMAKE_CURRENT_SOURCE_DIR}/client/ntp/*.c
${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
    device
    esp-tls
    esp_http_server
    json
    esp_event
)