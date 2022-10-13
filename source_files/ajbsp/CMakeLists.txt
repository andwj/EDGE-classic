# AJBSP CMake Script
cmake_minimum_required(VERSION 3.2.2)

project(edge_ajbsp)

set(CMAKE_CXX_FLAGS "-O2 -fno-strict-aliasing -fwrapv -fno-exceptions -fno-rtti")

add_library(edge_ajbsp
	bsp_level.cc
	bsp_misc.cc
	bsp_node.cc
	bsp_parse.cc
	bsp_utility.cc
	bsp_wad.cc
)
