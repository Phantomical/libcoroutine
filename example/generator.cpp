#define GENERATOR_IMPL

#include "generator.h"

#include "coroutine.h"

namespace impl
{
	void* yield(context* ctx, void* ptr)
	{
		return ::coroutine_yield(ctx, ptr);
	}
	void* next(context* ctx, void* ptr)
	{
		return ::coroutine_next(ctx, ptr);
	}
	bool is_complete(const context* ctx)
	{
		return ::coroutine_is_complete(ctx) != 0;
	}

	context* start(void(*func)(void*), size_t stack_size)
	{
		return ::coroutine_start({ stack_size, func });
	}
	context* start(void(*func)(void*), size_t stack_size, void* stackmem)
	{
		return ::coroutine_start_with_mem({ stack_size, func }, stackmem);
	}
}
