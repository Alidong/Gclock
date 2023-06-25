file(GLOB_RECURSE SOURCES 
"pal.c"
"pal_driver.c"
${CMAKE_CURRENT_SOURCE_DIR}/bus/*.c
${CMAKE_CURRENT_SOURCE_DIR}/lcd/*.c
${CMAKE_CURRENT_SOURCE_DIR}/knob/*.c
${CMAKE_CURRENT_SOURCE_DIR}/wifi/*.c
${CMAKE_CURRENT_SOURCE_DIR}/storage/*.c
${CMAKE_CURRENT_SOURCE_DIR}/bat/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/aht10/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/mic/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "." 
    PRIV_REQUIRES driver 
    knob
    button
    util
    esp_lcd 
    esp_wifi 
    esp_adc
    wpa_supplicant 
    nvs_flash
    spiffs
    vfs
    wear_levelling
    esp-dsp
    fatfs
    sdmmc
)