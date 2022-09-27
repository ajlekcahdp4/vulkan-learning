add_executable (02_instance 02_instance.cc)

target_include_directories (02_instance 
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(02_instance PRIVATE glfw)
target_link_libraries(02_instance PRIVATE Vulkan::Vulkan)
install (TARGETS 02_instance RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 02_instance)
