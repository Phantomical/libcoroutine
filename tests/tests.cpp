#include "coroutine.h"
#include "gtest\gtest.h"

#include <utility>

#define TEST_COROUTINE_STACK_SIZE 16384

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

	(void)coroutine_yield(vals->first, nullptr);

	*vals->second = 0xFFF;
}
void set_var_before(void* arg)
{
	std::pair<context*, int*>* vals = (std::pair<context*, int*>*)arg;
	*vals->second = 0xFFF;
}

TEST(run, yield_unmodified)
{
	context* ctx = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &deterministic_test });

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
	pair.first = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &set_var });
	pair.second = &r;

	coroutine_next(pair.first, &pair);

	coroutine_destroy(pair.first, nullptr);

	ASSERT_EQ(r, 0xFFF);
}
TEST(run, abort_does_not_complete)
{
	int r = 0;
	std::pair<context*, int*> pair;
	pair.first = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &set_var });
	pair.second = &r;

	coroutine_next(pair.first, &pair);

	coroutine_abort(pair.first);

	ASSERT_NE(r, 0xFFF);
}
TEST(run, is_complete)
{
	context* ctx = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &deterministic_test });

	EXPECT_EQ(0, reinterpret_cast<uintptr_t>(coroutine_next(ctx, ctx)));

	for (uintptr_t i = 1; i < 64; ++i)
	{
		EXPECT_EQ(i, reinterpret_cast<uintptr_t>(
			coroutine_next(ctx, reinterpret_cast<void*>(i))
			));
		EXPECT_TRUE(coroutine_is_complete(ctx) != 0);
	}
	(void)coroutine_next(ctx, nullptr);

	EXPECT_TRUE(coroutine_is_complete(ctx) != 0);
	coroutine_destroy(ctx, nullptr);
}
TEST(run, start_does_not_call_function)
{
	int i = 0;
	context* ctx = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &set_var_before });
	// Make sure that null is not returned
	ASSERT_NE(ctx, nullptr);
	std::pair<context*, int*> vals = { ctx, &i };
	// Make sure i has not changed
	ASSERT_EQ(i, 0);
	coroutine_next(ctx, &vals);
	// Make sure i was set
	ASSERT_EQ(i, 0xFFF);

	coroutine_abort(ctx);
}
