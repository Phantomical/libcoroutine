#define GENERATOR_IMPL

#include "generator.h"

#include "coroutine.h"

namespace impl
{
	void* yield(context* ctx, void* ptr)
	{
		return ::yield(ctx, ptr);
	}
	void* next(context* ctx, void* ptr)
	{
		return ::next(ctx, ptr);
	}
	bool is_complete(const context* ctx)
	{
		return ::is_complete(ctx) != 0;
	}

	context* start(void(*func)(void*), size_t stack_size)
	{
		return ::start({ stack_size, func });
	}
	context* start(void(*func)(void*), size_t stack_size, void* stackmem)
	{
		return ::start_with_mem({ stack_size, func }, stackmem);
	}
}
