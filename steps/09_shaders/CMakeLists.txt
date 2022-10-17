add_executable (09_shaders main.cc)

target_include_directories (09_shaders
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(09_shaders PRIVATE glfw)
target_link_libraries(09_shaders PRIVATE Vulkan::Vulkan)
install (TARGETS 09_shaders RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 09_shaders)