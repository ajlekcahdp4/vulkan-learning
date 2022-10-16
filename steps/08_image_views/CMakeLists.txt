add_executable (08_image_view main.cc)

target_include_directories (08_image_view
    PUBLIC ${GLFW_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries(08_image_view PRIVATE glfw)
target_link_libraries(08_image_view PRIVATE Vulkan::Vulkan)
install (TARGETS 08_image_view RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 08_image_view)