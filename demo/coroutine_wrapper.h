#pragma once

#include "coroutine.h"
#include <tuple>

struct coroutine_wrapper
{
private:
	context* ctx;
	void(*func)(coroutine_wrapper&);

	static void coroutine_base(void* ptr)
	{
		coroutine_wrapper* c = (coroutine_wrapper*)ptr;

		c->yield(0);

		c->func(*c);
	}

public:
	void yield(size_t val)
	{
		::yield(ctx, &val);
	}
	size_t next()
	{
		return *(size_t*)::next(ctx, nullptr);		
	}

	bool complete() const
	{
		return ::is_complete(ctx);
	}

	coroutine_wrapper(size_t stack_size, void(*func)(coroutine_wrapper&)) :
		ctx(nullptr),
		func(func)
	{
		ctx = ::start({ stack_size, &coroutine_base });

		::next(ctx, nullptr);
	}
	~coroutine_wrapper()
	{
		::destroy(ctx, nullptr);
	}
};
