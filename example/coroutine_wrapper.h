#pragma once

#include "coroutine.h"
#include <tuple>

struct coroutine_wrapper
{
private:
	coroutine* ctx;
	void(*func)(coroutine_wrapper&);
	size_t* ptr;

	static void coroutine_base(void* ptr)
	{
		coroutine_wrapper* c = (coroutine_wrapper*)ptr;

		c->yield(0);

		c->func(*c);
	}

public:
	void yield(size_t val)
	{
		coroutine_yield(ctx, &val);
	}
	bool next()
	{
		ptr = (size_t*)coroutine_next(ctx, nullptr);
		return !complete();
	}
	size_t value() const
	{
		return *ptr;
	}

	bool complete() const
	{
		return !!coroutine_is_complete(ctx);
	}
	
	coroutine_wrapper(size_t stack_size, void(*func)(coroutine_wrapper&)) :
		ctx(nullptr),
		func(func)
	{
		ctx = coroutine_start({ stack_size, &coroutine_base });

		coroutine_next(ctx, this);
	}
	coroutine_wrapper(size_t stack_size, void(*func)(coroutine_wrapper&), void* mem) :
		ctx(nullptr),
		func(func)
	{
		ctx = coroutine_start_with_mem({ stack_size, &coroutine_base }, mem);

		coroutine_next(ctx, this);
	}
	~coroutine_wrapper()
	{
		coroutine_destroy(ctx, nullptr);
	}
};
