add_executable (01_integrating_GLFW 01_integrating_GLFW.cc)
target_link_libraries(01_integrating_GLFW PRIVATE glfw)
install (TARGETS 01_integrating_GLFW RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin COMPONENT 01_integrating_GLFW)

