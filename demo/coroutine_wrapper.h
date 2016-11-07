#pragma once

#include "coroutine.h"
#include <tuple>

struct coroutine
{
private:
	context* ctx;
	void(*func)(coroutine&);

	static void coroutine_base(void* ptr)
	{
		coroutine* c = (coroutine*)ptr;

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

	coroutine(size_t stack_size, void(*func)(coroutine&)) :
		ctx(nullptr),
		func(func)
	{
		ctx = ::start({ stack_size, &coroutine_base });

		::next(ctx, nullptr);
	}
	~coroutine()
	{
		::destroy(ctx, nullptr);
	}
};
