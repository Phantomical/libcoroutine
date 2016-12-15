#include "coroutine.h"
#include "gtest\gtest.h"

void deterministic_test(void* arg)
{
	context* ctx = static_cast<context*>(arg);
	for (uintptr_t i = 0; i < 64; ++i)
	{
		coroutine_yield(ctx, reinterpret_cast<void*>(i));
	}
}

TEST(run, yield_unmodified)
{
	context* ctx = coroutine_start({ 16382, &deterministic_test });

	EXPECT_EQ(0, reinterpret_cast<uintptr_t>(coroutine_next(ctx, ctx)));

	for (uintptr_t i = 1; i < 64; ++i)
	{
		EXPECT_EQ(i, reinterpret_cast<uintptr_t>(
			coroutine_next(ctx, reinterpret_cast<void*>(i))
		));
	}
}
