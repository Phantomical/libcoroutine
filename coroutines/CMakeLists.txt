# CMake build script for asm subproject

cmake_minimum_required(VERSION 3.0)

set(sources none)
set(c_sources coroutine.c)
set(can_use_assembler FALSE)

if ("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "i.86")
	enable_language(ASM_MASM)
	if (CMAKE_ASM_MASM_COMPILER_WORKS)
		set(can_use_assembler TRUE)
		set(sources stack32.asm)
	endif (CMAKE_ASM_MASM_COMPILER_WORKS)
elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "AMD64")
	enable_language(ASM_MASM)
	if (CMAKE_ASM_MASM_COMPILER_WORKS)
		set(can_use_assembler TRUE)
		set(sources stack64.asm)
	endif (CMAKE_ASM_MASM_COMPILER_WORKS)
endif()

if (NOT can_use_assembler)
	message(FATAL_ERROR "MASM assembler is required for building the coroutine library.")
endif()

add_library(coroutine STATIC ${sources} ${c_sources})

target_include_directories(coroutine PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

