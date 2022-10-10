add_executable (07_swapchain main.cc)

target_include_directories (07_swapchain
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(07_swapchain PRIVATE glfw)
target_link_libraries(07_swapchain PRIVATE Vulkan::Vulkan)
install (TARGETS 07_swapchain RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 07_swapchain)
