# Build script for demo coroutine app

cmake_minimum_required(VERSION 2.6)

file(GLOB sources
	"*.cpp"
	"*.h"
)

add_executable(coroutine-example ${sources})

target_include_directories(coroutine-example PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)
target_link_libraries(coroutine-example coroutine)
