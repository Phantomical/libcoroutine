# CMake file for tests directory 

cmake_minimum_required(VERSION 3.7)

enable_testing()
find_package(GTest QUIET)

file(GLOB srcs
	"*.cpp"
	"*.h"
)

add_executable(tests ${srcs})

if (GTEST_FOUND)
	target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIRS})
	target_link_libraries(tests ${GTEST_BOTH_LIBRARIES})
else()
	include(ExternalProject)
	
	set(CMAKE_INSTALL_LOCAL_ONLY TRUE)
	
	set(GTEST_INSTALL_DIR "${CMAKE_BINARY_DIR}/deps/gtest/install")

	ExternalProject_Add(gtest
		PREFIX "${CMAKE_BINARY_DIR}/deps/gtest"
		INSTALL_DIR ${GTEST_INSTALL_DIR}
		#CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}"
		GIT_REPOSITORY https://github.com/google/googletest.git
	)
	
	target_include_directories(tests PRIVATE ${GTEST_INSTALL_DIR})
	#target_link_libraries
endif()

