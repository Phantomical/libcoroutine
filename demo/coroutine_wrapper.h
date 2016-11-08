#pragma once

#include "coroutine.h"
#include <tuple>

struct coroutine_wrapper
{
private:
	context* ctx;
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
		::yield(ctx, &val);
	}
	bool next()
	{
		ptr = (size_t*)::next(ctx, nullptr);
		return !complete();
	}
	size_t value() const
	{
		return *ptr;
	}

	bool complete() const
	{
		return !!::is_complete(ctx);
	}
	
	coroutine_wrapper(size_t stack_size, void(*func)(coroutine_wrapper&)) :
		ctx(nullptr),
		func(func)
	{
		ctx = ::start({ stack_size, &coroutine_base });

		::next(ctx, this);
	}
	coroutine_wrapper(size_t stack_size, void(*func)(coroutine_wrapper&), void* mem) :
		ctx(nullptr),
		func(func)
	{
		ctx = ::start_with_mem({ stack_size, &coroutine_base }, mem);

		::next(ctx, this);
	}
	~coroutine_wrapper()
	{
		::destroy(ctx, nullptr);
	}
};
