#include "coroutine.h"
#include "gtest\gtest.h"

#include <utility>

#define TEST_COROUTINE_STACK_SIZE 16384

struct continue_data
{
	coroutine* next;
	bool completed;
};

void continue_func(coroutine* ctx, void* datap)
{
	continue_data* data = (continue_data*)datap;
	coroutine_yield(ctx, nullptr);
	for (size_t i = 0; i < 4; ++i)
	{
		coroutine_continue(ctx, data->next, nullptr);
	}

	data->completed = true;
}

TEST(continue, sanity_check)
{
	coroutine* co1 = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &continue_func });
	coroutine* co2 = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &continue_func });

	continue_data dat1 = { co2, false };
	continue_data dat2 = { co1, false };

	coroutine_next(co1, &dat1);
	coroutine_next(co2, &dat2);

	coroutine_next(co1, nullptr);

	// co1 should have completed
	coroutine_destroy(co1, nullptr);

	// co2 should not have complete yet
	// since it yielded to co1 before it finished

	EXPECT_TRUE(dat1.completed);
	EXPECT_FALSE(dat2.completed);

	coroutine_next(co2, nullptr);

	// co2 should be complete by now
	EXPECT_TRUE(dat2.completed);

	coroutine_destroy(co2, nullptr);
}

TEST(continue, returns_null_on_null_ctx)
{
	coroutine* co2 = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &continue_func });

	EXPECT_EQ(nullptr, coroutine_continue(nullptr, co2, nullptr));

	coroutine_abort(co2);

}
TEST(continue, returns_null_on_null_next)
{
	coroutine* co1 = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &continue_func });

	EXPECT_EQ(nullptr, coroutine_continue(co1, nullptr, nullptr));

	coroutine_abort(co1);
}

TEST(continue, continue_to_self_works)
{
	coroutine* ctx = coroutine_start({ TEST_COROUTINE_STACK_SIZE, &continue_func });

	continue_data dat = { ctx, false };
	(void)coroutine_next(ctx, &dat);
	// Have to call next twice since continue_func yields
	(void)coroutine_next(ctx, nullptr);

	EXPECT_TRUE(dat.completed);

	coroutine_destroy(ctx, nullptr);
}
