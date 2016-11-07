#include "coroutine.h"

#include <stdbool.h>
#include <malloc.h>

#if defined(_WIN32) && !defined(_WIN64)
#	define CALL_CONV __fastcall
#elif defined(_WIN64)
#	define CALL_CONV
#else
#	define CALL_CONV
#endif

// Switch to the given stack
// new_stack_ptr: the top of the stack to switch to
// old_stack_ptr: the current top of the stack
void CALL_CONV jmp_stack(void* new_stack_ptr, void** old_stack_ptr);

// Information used to initialize the coroutine
// this data will not persist beyond the first
// yield
typedef struct _tmpinfo
{
	void* stack_ptr;
	void(*internalfunc)(struct _tmpinfo*);
	void(*funcptr)(void*);
	context* ctx;
} tmpinfo;

void CALL_CONV init_stack(tmpinfo* info, void** old_stack_ptr);

struct _context
{
	char complete;
	struct _x1
	{
		void* stack_pointer;
		void* stack_start;
	} coroutine;
	struct _x2
	{
		void* stack_pointer;
	} caller;
	void* datap;
};

void coroutine_init(tmpinfo* info)
{
	// info stops being valid after we call yield
	// so we have to save ctx now
	context* ctx = info->ctx;
	// Same with funcptr
	void(*funcptr)(void*) = info->funcptr;

	// Yield and get the first value from the caller
	void* datap = yield(ctx, NULL);

	// Now that the caller has called next
	// we can execute the coroutine method
	funcptr(datap);

	// Inticate that the coroutine has completed
	ctx->complete = true;
}

void* yield(context* ctx, void* datap)
{
	ctx->datap = datap;
	jmp_stack(ctx->caller.stack_pointer, &ctx->coroutine.stack_pointer);
	return ctx->datap;
}

void* next(context* ctx, void* datap)
{
	if (!is_complete(ctx))
	{
		ctx->datap = datap;
		jmp_stack(ctx->coroutine.stack_pointer, &ctx->caller.stack_pointer);
	}
	return ctx->datap;
}

char is_complete(const context* ctx)
{
	return ctx->complete;
}

context* start(coroutine initdata, void* datap)
{
	context* ctx = malloc(sizeof(context));
	ctx->coroutine.stack_start = malloc(initdata.stack_size);
	ctx->coroutine.stack_pointer = NULL;
	ctx->caller.stack_pointer = NULL;
	ctx->complete = false;
	ctx->datap = datap;

	tmpinfo info = {
		ctx->coroutine.stack_start,
		&coroutine_init,
		initdata.funcptr,
		ctx
	};
}
