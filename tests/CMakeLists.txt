# CMake file for tests directory 

cmake_minimum_required(VERSION 2.8)

enable_testing()
find_package(GTest QUIET)

file(GLOB srcs
	"*.cpp"
	"*.h"
)

add_executable(tests ${srcs})

target_link_libraries(tests coroutine)

if (GTEST_FOUND)
	target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIRS})
	target_link_libraries(tests ${GTEST_LIBRARIES})
else()
	include(ExternalProject)
	
	set(CMAKE_INSTALL_LOCAL_ONLY TRUE)
	
	set(GTEST_INSTALL_DIR "${CMAKE_BINARY_DIR}/deps/gtest/install")
	
	ExternalProject_Add(gtest
		PREFIX "${CMAKE_BINARY_DIR}/deps/gtest"
		INSTALL_DIR ${GTEST_INSTALL_DIR}
		CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}"
		GIT_REPOSITORY https://github.com/google/googletest.git
	)
		
	target_include_directories(tests PRIVATE ${GTEST_INSTALL_DIR}/include)
	
	target_link_libraries(tests 
		"${GTEST_INSTALL_DIR}/lib/${CMAKE_STATIC_MODULE_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
	)
	add_dependencies(tests gtest)
endif()

add_dependencies(tests coroutine)

foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
   if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)

add_test(libcoroutine-tests tests $<TARGET_FILE:tests>)

