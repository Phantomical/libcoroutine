#include "coroutine.h"
#include "gtest\gtest.h"

#include <utility>

#define TEST_COROUTINE_STACK_SIZE 16384

struct recursive_data
{
	coroutine* self;
	coroutine* next;
};

