add_executable (04_physical_devices main.cc)

target_include_directories (04_physical_devices
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(04_physical_devices PRIVATE glfw)
target_link_libraries(04_physical_devices PRIVATE Vulkan::Vulkan)
install (TARGETS 04_physical_devices RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 04_physical_devices)
