#include "coroutine.h"

#include <stdbool.h>
#include <malloc.h>
#include <assert.h>

#if defined(_WIN32) && !defined(_WIN64)
#	define CALL_CONV __fastcall
#elif defined(_WIN64)
#	define CALL_CONV
#else
#	define CALL_CONV
#endif

/* Adjustment procedures for various architectures (Only necessary if the stack grows down) */
#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
// AMD64 (x86-64) architecture
#define ADJUST_SP(size, sp) ((void*)((char*)sp + size))
#define VALID_SP(s_st, sp) ((uintptr_t)sp > (uintptr_t)s_st)
#elif defined (__i386__) || defined(__i386) || defined(_M_IX86) || defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) || defined(__386)
// x86 architecture
#define ADJUST_SP(size, sp) ((void*)((char*)sp + size))
#define VALID_SP(s_st, sp) ((uintptr_t)sp > (uintptr_t)s_st)
#else
#error "Architectures other than x86 or x86-64 are not supported"
#endif

// Information used to initialize the coroutine
// this data will not persist beyond the first
// yield
#pragma pack(push)
#pragma pack(1)
typedef struct _tmpinfo
{
	void* stack_base;
	void(CALL_CONV *internalfunc)(struct _tmpinfo*);
	void(*funcptr)(void*);
	context* ctx;
} tmpinfo;
#pragma pack(pop)

struct _context
{
	char complete : 1;
	char external_mem : 1;
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

/* ASM Routines */
// Switch to the given stack
// new_stack_ptr: the top of the stack to switch to
// old_stack_ptr: the current top of the stack
void CALL_CONV jmp_stack(void* new_stack_ptr, void** old_stack_ptr);

void CALL_CONV init_stack(const tmpinfo* info, void** old_stack_ptr);

/* C routines */

void CALL_CONV coroutine_init(const tmpinfo* info)
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

	// Indicate that the coroutine has completed
	ctx->complete = true;
}

void* yield(context* ctx, void* datap)
{
	// If this assert triggers then the coroutine stack has overflowed
	// there isn't really any recovery that can be done here.
	// The only solution is to increase the stack size of the 
	// coroutine at creation time.
	assert(ctx->coroutine.stack_pointer == NULL || VALID_SP(ctx->coroutine.stack_start, ctx->coroutine.stack_pointer));

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

context* start(coroutine initdata)
{
	context* ctx = start_with_mem(initdata, malloc(initdata.stack_size));
	ctx->external_mem = false;
	return ctx;
}
context* start_with_mem(coroutine initdata, void* stackmem)
{
	context* ctx = malloc(sizeof(context));
	ctx->coroutine.stack_start = stackmem;
	ctx->coroutine.stack_pointer = NULL;
	ctx->caller.stack_pointer = NULL;
	ctx->complete = false;
	ctx->external_mem = true;
	ctx->datap = NULL;

	tmpinfo info = {
		ADJUST_SP(initdata.stack_size, ctx->coroutine.stack_start),
		&coroutine_init,
		initdata.funcptr,
		ctx
	};

	init_stack(&info, &ctx->caller.stack_pointer);

	return ctx;
}
void destroy(context* ctx, void* datap)
{
	// Keep executing the coroutine until it is finished
	while (!is_complete(ctx))
	{
		next(ctx, datap);
	}

	// Free up resources
	abort_c(ctx);
}
void abort_c(context* ctx)
{
	if (!ctx->external_mem)
		free(ctx->coroutine.stack_start);
	free(ctx);
}
