add_executable (04_phys_devices_and_queu 04_phys_devices_and_queue.cc)

target_include_directories (04_phys_devices_and_queu 
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(04_phys_devices_and_queu PRIVATE glfw)
target_link_libraries(04_phys_devices_and_queu PRIVATE Vulkan::Vulkan)
install (TARGETS 04_phys_devices_and_queu RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 04_phys_devices_and_queu)
