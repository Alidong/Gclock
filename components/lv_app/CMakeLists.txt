file(GLOB_RECURSE SOURCES 
"lv_port.c"
"lv_app.c"
${CMAKE_CURRENT_SOURCE_DIR}/pageManager/*.c
${CMAKE_CURRENT_SOURCE_DIR}/pages/*.c
${CMAKE_CURRENT_SOURCE_DIR}/font/*.c
${CMAKE_CURRENT_SOURCE_DIR}/icon/*.c
${CMAKE_CURRENT_SOURCE_DIR}/hal/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES pal lvgl esp_lcd esp_timer vfs util 
)