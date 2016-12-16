# CMake build script for libcoroutine

cmake_minimum_required(VERSION 3.0)

set(sources none)
set(c_sources coroutine.c)
set(include_file "${CMAKE_CURRENT_SOURCE_DIR}/../include/coroutine.h")
set(can_use_assembler FALSE)

include(TargetArch.cmake)

target_architecture(architecture)

if ("${architecture}" MATCHES "i.86")
	enable_language(ASM_MASM)
	if (CMAKE_ASM_MASM_COMPILER_WORKS)
		set(can_use_assembler TRUE)
		set(sources stack32.asm)
		set_source_files_properties(stack32.asm PROPERTIES COMPILE_FLAGS "/safeseh")
	endif (CMAKE_ASM_MASM_COMPILER_WORKS)
elseif("${architecture}" MATCHES "x86_64")
	enable_language(ASM_MASM)
	if (CMAKE_ASM_MASM_COMPILER_WORKS)
		set(can_use_assembler TRUE)
		set(sources stack64.asm)
	endif (CMAKE_ASM_MASM_COMPILER_WORKS)
else()
	message(FATAL_ERROR "Unsupported target architecture!")
endif()

if (NOT can_use_assembler)
	message(FATAL_ERROR "MASM assembler is required for building the coroutine library.")
endif()

add_library(coroutine STATIC ${sources} ${c_sources} ${include_file})

target_include_directories(coroutine PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

