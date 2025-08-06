idf_component_register(SRCS "main.cpp" "simple_object_pool.cpp" "little_dawn_object_pool.cpp" "minimal_object_pool.cpp" "working_object_pool.cpp"
                       PRIV_REQUIRES spi_flash esp_driver_gpio driver AgIsoStack
                       INCLUDE_DIRS "")

target_add_binary_data(${COMPONENT_TARGET} "object_pool.iop" BINARY)
