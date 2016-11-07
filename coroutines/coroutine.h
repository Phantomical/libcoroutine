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
	// passing datap back to the caller
	void yield(void* datap, context* ctx);

	// Starts a coroutine with a given context
	// and executes until the first yield call
	context* start(coroutine coroutine, void* datap);
	// Performs context cleanup and completes execution
	// of the coroutine
	void destroy(context* ctx, void* datap);

	// Executes the coroutine to the next yield call
	// with a provided data pointer
	void* next(context* ctx, void* datap);

	// Returns 1 if the coroutine is complete, 0 otherwise
	char is_complete(context* ctx);
#ifdef __cplusplus
}
#endif
