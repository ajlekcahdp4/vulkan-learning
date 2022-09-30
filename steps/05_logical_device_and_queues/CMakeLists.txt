add_executable (05_logical_device_and_queues 05_logical_device_and_queues.cc)

target_include_directories ( 05_logical_device_and_queues
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(05_logical_device_and_queues PRIVATE glfw)
target_link_libraries(05_logical_device_and_queues PRIVATE Vulkan::Vulkan)
install (TARGETS 05_logical_device_and_queues RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 05_logical_device_and_queues)
