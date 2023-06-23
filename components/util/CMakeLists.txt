file(GLOB_RECURSE SOURCES 
"util.c"
# ${CMAKE_CURRENT_SOURCE_DIR}/bus/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/lcd/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/button/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/net/*.c
# # ${CMAKE_CURRENT_SOURCE_DIR}/storage/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/power/*.c
# ${CMAKE_CURRENT_SOURCE_DIR}/aht10/*.c
)
idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
    pal
    app
    # esp_lcd 
    # esp_wifi 
    # wpa_supplicant 
    # nvs_flash
    # fatfs
    # vfs
    # wear_levelling
)