#include "coroutine.h"
#include "gtest\gtest.h"

#include <utility>

void deterministic_test(void* arg)
{
	context* ctx = static_cast<context*>(arg);
	for (uintptr_t i = 0; i < 64; ++i)
	{
		coroutine_yield(ctx, reinterpret_cast<void*>(i));
	}
}

void set_var(void* arg)
{
	std::pair<context*, int*>* vals = (std::pair<context*, int*>*)arg;

	coroutine_yield(vals->first, nullptr);

	*vals->second = 0xFFF;
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

	coroutine_abort(ctx);
}

TEST(run, destroy_completes_coroutine)
{
	int r = 0;
	std::pair<context*, int*> pair;
	pair.first = coroutine_start({ 16382, &set_var });
	pair.second = &r;

	coroutine_next(pair.first, &pair);

	coroutine_destroy(pair.first, nullptr);

	ASSERT_EQ(r, 0xFFF);
}

TEST(run, abort_does_not_complete)
{
	int r = 0;
	std::pair<context*, int*> pair;
	pair.first = coroutine_start({ 16382, &set_var });
	pair.second = &r;

	coroutine_next(pair.first, &pair);

	coroutine_abort(pair.first);

	ASSERT_NE(r, 0xFFF);
}

TEST(run, is_complete_works)
{
	context* ctx = coroutine_start({ 16382, &deterministic_test });

	EXPECT_EQ(0, reinterpret_cast<uintptr_t>(coroutine_next(ctx, ctx)));

	for (uintptr_t i = 1; i < 64; ++i)
	{
		EXPECT_EQ(i, reinterpret_cast<uintptr_t>(
			coroutine_next(ctx, reinterpret_cast<void*>(i))
			));
		EXPECT_EQ((bool)coroutine_is_complete(ctx), !(i < 63));
	}

	coroutine_abort(ctx);
}
