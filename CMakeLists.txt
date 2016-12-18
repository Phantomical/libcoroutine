# Global CMake script for coroutine library

cmake_minimum_required(VERSION 2.8)

project(libcoroutine)

set(BUILD_TESTS TRUE CACHE BOOL "Whether to build the tests for the library.")

enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/include)

# Library Dir
add_subdirectory(coroutine)

# Tests
if (BUILD_TESTS)
	add_subdirectory(tests)
endif()
