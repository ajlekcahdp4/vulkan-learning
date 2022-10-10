add_executable (06_window_surface main.cc)

target_include_directories (06_window_surface
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(06_window_surface PRIVATE glfw)
target_link_libraries(06_window_surface PRIVATE Vulkan::Vulkan)
install (TARGETS 06_window_surface RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 06_window_surface)
