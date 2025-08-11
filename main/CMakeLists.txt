idf_component_register(SRCS "main.cpp" "new_dawn_serial.cpp" "manual_pool.cpp"
                       PRIV_REQUIRES spi_flash esp_driver_gpio driver AgIsoStack
                       INCLUDE_DIRS "")

target_add_binary_data(${COMPONENT_TARGET} "LD19.iop" BINARY)
