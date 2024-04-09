option(BUILD_SHARED_LIBS "Build simple_socket shared library" OFF)

add_library(simple_socket)

target_compile_features(simple_socket PUBLIC cxx_std_11)

if(WIN32)
  target_link_libraries(simple_socket wsock32 ws2_32)
endif()

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
  EXPORT simple_socketTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/simple_socket
  DESTINATION include
)

install(
  EXPORT simple_socketTargets
  FILE simple_socketTargets.cmake
  NAMESPACE simple_socket::
  DESTINATION lib/cmake/simple_socket
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/simple_socketConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simple_socket
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/simple_socketConfigVersion.cmake"
  VERSION "${simple_socket_VERSION_MAJOR}.${simple_socket_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

install(
  FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/simple_socketConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/simple_socketConfigVersion.cmake"
  DESTINATION lib/cmake/simple_socket
)

export(
  EXPORT simple_socketTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/simple_socketTargets.cmake"
  NAMESPACE simple_socket::
)