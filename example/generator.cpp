#define GENERATOR_IMPL

#include "generator.h"

#include "coroutine.h"

namespace impl
{
	void* yield(coroutine* ctx, void* ptr)
	{
		return ::coroutine_yield(ctx, ptr);
	}
	void* next(coroutine* ctx, void* ptr)
	{
		return ::coroutine_next(ctx, ptr);
	}
	bool is_complete(const coroutine* ctx)
	{
		return ::coroutine_is_complete(ctx) != 0;
	}

	coroutine* start(void(*func)(void*), size_t stack_size)
	{
		return ::coroutine_start({ stack_size, func });
	}
	coroutine* start(void(*func)(void*), size_t stack_size, void* stackmem)
	{
		return ::coroutine_start_with_mem({ stack_size, func }, stackmem);
	}
}
