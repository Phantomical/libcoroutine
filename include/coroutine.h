#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct _coroutine coroutine;
	typedef struct
	{
		size_t stack_size;
		void(*funcptr)(coroutine* ctx, void* datap);
	} coroutine_data;

	// Yields the coroutine and returns control to the caller
	// passing datap back to the caller. The return value is
	// the pointer that was passed into the coroutine.
	void* coroutine_yield(coroutine* ctx, void* datap);

	// Creates a coroutine with a given context but doesn't begin executing it.
	coroutine* coroutine_start(coroutine_data initdata);
	// Starts a coroutine using the an external buffer for
	// the stack space of the coroutine
	// NOTE: If initdata.stack_size is greater than the size of the buffer
	// then data before the buffer will be overwritten (on x86 and x86-64)
	coroutine* coroutine_start_with_mem(coroutine_data initdata, void* stackmem);
	// Performs context cleanup and completes execution
	// of the coroutine
	void coroutine_destroy(coroutine* ctx, void* datap);
	// Performs context cleanup without completing
	// execution of the coroutine.
	// WARNING: Calling this method will cause the
	//   coroutine to never complete execution, this
	//   can cause memory leaks and essential cleanup
	//   to never be executed.
	void coroutine_abort(coroutine* ctx);

	// Executes the coroutine to the next yield call
	// with a provided data pointer. If the coroutine
	// is completed then it returns the last yielded
	// value instead.
	void* coroutine_next(coroutine* ctx, void* datap);

	// Returns 1 if the coroutine is complete, 0 if it is not
	// and -1 if ctx is NULL
	char coroutine_is_complete(const coroutine* ctx);

	void* coroutine_continue(coroutine* ctx, coroutine* next, void* datap);

	/* Unsafe API */

	// Performs the same operation as coroutine_yield but
	// doesn't perform any sanity checks and will cause 
	// undefined behaviour when given a coroutine that is
	// not valid.
	void* coroutine_unsafe_yield(coroutine* ctx, void* datap);
	// Performs the same operation as coroutine_next but
	// doesn't check to see if ctx is NULL or that the
	// coroutine has finished executing.
	void* coroutine_unsafe_next(coroutine* ctx, void* datap);
#ifdef __cplusplus
}
#endif
