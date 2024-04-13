cmake_minimum_required(VERSION 3.8)

project(simple_socket VERSION 1.0 DESCRIPTION "Simple socket implementation for C++" LANGUAGES CXX)

option(Demo "Demo" OFF)

include(GNUInstallDirs)

add_subdirectory(src)

if (Demo)
    add_subdirectory(demo)
endif()