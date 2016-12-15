#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct _context context;
	typedef struct _coroutine
	{
		size_t stack_size;
		void(*funcptr)(void* datap);
	} coroutine;

	// Yields the coroutine and returns control to the caller
	// passing datap back to the caller. The return value is
	// the pointer that was passed into the coroutine.
	void* coroutine_yield(context* ctx, void* datap);

	// Starts a coroutine with a given context but doesn't begin executing it.
	context* coroutine_start(coroutine initdata);
	// Starts a coroutine using the an external buffer for
	// the stack space of the coroutine
	// NOTE: If initdata.stack_size is greater than the size of the buffer
	// then data before the buffer will be overwritten (on x86 and x86-64)
	context* coroutine_start_with_mem(coroutine initdata, void* stackmem);
	// Performs context cleanup and completes execution
	// of the coroutine
	void coroutine_destroy(context* ctx, void* datap);
	// Performs context cleanup without completing
	// execution of the coroutine.
	// WARNING: Calling this method will cause the
	//   coroutine to never complete execution, this
	//   can cause memory leaks and essential cleanup
	//   to never be executed.
	void coroutine_abort(context* ctx);

	// Executes the coroutine to the next yield call
	// with a provided data pointer. If the coroutine
	// is completed then it returns the last yielded
	// value instead.
	void* coroutine_next(context* ctx, void* datap);

	// Returns 1 if the coroutine is complete, 0 otherwise.
	char coroutine_is_complete(const context* ctx);
#ifdef __cplusplus
}
#endif
