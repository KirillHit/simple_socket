option(BUILD_SHARED_LIBS "Build simple_socket shared library" OFF)

add_library(simple_socket)

target_compile_features(simple_socket PUBLIC cxx_std_11)

target_include_directories(
    simple_socket
  PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)

target_sources(
    simple_socket
  PRIVATE 
    simple_socket.cpp
)

install(
  TARGETS simple_socket
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/simple_socket
  DESTINATION include
)