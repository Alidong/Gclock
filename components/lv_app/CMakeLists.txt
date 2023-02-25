file(GLOB_RECURSE SOURCES 
"lv_port.c"
"lv_app.c"
${CMAKE_CURRENT_SOURCE_DIR}/pagemanager/*.c
${CMAKE_CURRENT_SOURCE_DIR}/pages/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/xl9535/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES device lvgl esp_lcd esp_timer vfs
)