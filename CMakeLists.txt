cmake_minimum_required(VERSION 2.6)
project(game)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-find")

find_package(PkgConfig)
# GTKMM
pkg_check_modules(GTKMM REQUIRED gtkmm-2.4)
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

# GTKGLEXTMM
pkg_check_modules(GLEXT REQUIRED gtkglextmm-1.2)
include_directories(${GLEXT_INCLUDE_DIRS})
link_directories(${GLEXT_LIBRARY_DIRS})

find_package(GLEW)

#set(BOOST_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-g -Wall")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
 
add_subdirectory(src)
