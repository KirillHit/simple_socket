cmake_minimum_required(VERSION 2.8)

project(simple_socket)

option(Demo "Demo" OFF)

include(GNUInstallDirs)

add_subdirectory(src)

if (Demo)
    add_subdirectory(demo)
endif()