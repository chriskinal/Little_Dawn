# ESP-IDF component configuration for AgIsoStack++

# Define the component
idf_component_register(
    SRCS 
        # Core ISOBUS sources
        "isobus/src/can_NAME.cpp"
        "isobus/src/can_NAME_filter.cpp"
        "isobus/src/can_callbacks.cpp"
        "isobus/src/can_control_function.cpp"
        "isobus/src/can_extended_transport_protocol.cpp"
        "isobus/src/can_identifier.cpp"
        "isobus/src/can_internal_control_function.cpp"
        "isobus/src/can_message.cpp"
        "isobus/src/can_message_data.cpp"
        "isobus/src/can_message_frame.cpp"
        "isobus/src/can_network_configuration.cpp"
        "isobus/src/can_network_manager.cpp"
        "isobus/src/can_parameter_group_number_request_protocol.cpp"
        "isobus/src/can_partnered_control_function.cpp"
        "isobus/src/can_stack_logger.cpp"
        "isobus/src/can_transport_protocol.cpp"
        "isobus/src/can_transport_protocol_base.cpp"
        "isobus/src/isobus_virtual_terminal_client.cpp"
        "isobus/src/isobus_virtual_terminal_client_state_tracker.cpp"
        "isobus/src/isobus_virtual_terminal_client_update_helper.cpp"
        "isobus/src/isobus_virtual_terminal_objects.cpp"
        "isobus/src/isobus_heartbeat.cpp"
        "isobus/src/nmea2000_fast_packet_protocol.cpp"
        "isobus/src/isobus_language_command_interface.cpp"
        # Hardware integration
        "hardware_integration/src/can_hardware_interface.cpp"
        "hardware_integration/src/twai_plugin.cpp"
        # Utility
        "utility/src/system_timing.cpp"
        "utility/src/processing_flags.cpp"
    INCLUDE_DIRS 
        "isobus/include"
        "hardware_integration/include"
        "utility/include"
    PRIV_REQUIRES 
        driver
        esp_timer
        freertos
)

# Disable threading for ESP32
target_compile_definitions(${COMPONENT_LIB} PUBLIC CONFIG_CAN_STACK_DISABLE_THREADS=1)

# Enable C++17
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)