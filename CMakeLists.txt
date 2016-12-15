# Global CMake script for coroutine library

cmake_minimum_required(VERSION 2.6)

project(coroutines)

set(BUILD_EXAMPLE_APP TRUE CACHE BOOL "Whether to build the example app.")
set(BUILD_TESTS TRUE CACHE BOOL "Whether to build the tests for the library.")

enable_language(ASM_MASM)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Library Dir
add_subdirectory(coroutine)

# Demo App
if (BUILD_EXAMPLE_APP)
	add_subdirectory(example)
endif()

# Tests
if (BUILD_TESTS)
	add_subdirectory(tests)
endif()
