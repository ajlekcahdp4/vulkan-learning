add_executable (10_graphics_pipeline main.cc)

target_include_directories (10_graphics_pipeline
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(10_graphics_pipeline PRIVATE glfw)
target_link_libraries(10_graphics_pipeline PRIVATE Vulkan::Vulkan)
install (TARGETS 10_graphics_pipeline RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 10_graphics_pipeline)