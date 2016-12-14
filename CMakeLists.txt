# Global CMake script for coroutine library

cmake_minimum_required(VERSION 2.6)

project(coroutines)

set(BUILD_EXAMPLE_APP TRUE CACHE BOOL "Whether to build the example app.")

enable_language(ASM_MASM)

# Library Dir
add_subdirectory(coroutines)
# Demo App
if (BUILD_EXAMPLE_APP)
	add_subdirectory(demo)
endif()
