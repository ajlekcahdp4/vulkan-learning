add_executable (03_validation_layers 03_validation_layers.cc)

target_include_directories (03_validation_layers 
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(03_validation_layers PRIVATE glfw)
target_link_libraries(03_validation_layers PRIVATE Vulkan::Vulkan)
install (TARGETS 03_validation_layers RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 03_validation_layers)
