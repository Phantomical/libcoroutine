#include "coroutine.h"

#include <stdbool.h>

struct _context
{
	bool complete;
	struct _x1
	{
		void* stack_pointer;
		void* stack_start;
	} coroutine;
	struct _x2
	{
		void* stack_pointer;
	} caller;
};
