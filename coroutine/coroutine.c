#include "coroutine.h"

#include <stdbool.h>
#include <malloc.h>
#include <assert.h>

#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#	define ARCH_X86_64
#elif defined (__i386__) || defined(__i386) || defined(_M_IX86) || defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) || defined(__386)
#	define ARCH_X86
#else
#error "Architectures other than x86 or x86-64 are not supported at this time"
#endif

#if defined(_WIN32) && !defined(_WIN64)
#	define CALL_CONV __fastcall
#elif defined(_WIN64)
#	define CALL_CONV
#elif (defined __GNUC__ || defined __clang__) && defined ARCH_X86
#	define CALL_CONV __fastcall
#elif (defined __GNUC__ || defined __clang__) && defined ARCH_X86_64
#	define CALL_CONV ms_abi
#else
#	define CALL_CONV
#endif

/* Adjustment procedures for various architectures (Only necessary if the stack grows down) */
#if defined(ARCH_X86_64)
// AMD64 (x86-64) architecture
#define ADJUST_SP(size, sp) ((void*)((char*)sp + size))
#define VALID_SP(s_st, sp) ((uintptr_t)sp > (uintptr_t)s_st)
#elif defined (ARCH_X86)
// x86 architecture
#define ADJUST_SP(size, sp) ((void*)((char*)sp + size))
#define VALID_SP(s_st, sp) ((uintptr_t)sp > (uintptr_t)s_st)
#endif

// Information used to initialize the coroutine
// this data will not persist beyond the first
// yield
#pragma pack(push)
// Make sure it is arranged in packed bytes
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
	char complete;
	char external_mem;
};

/* ASM Routines */
// Switch to the given stack
// new_stack_ptr: the top of the stack to switch to
// old_stack_ptr: the current top of the stack
extern void CALL_CONV coroutine_jmp_stack(void* new_stack_ptr, void** old_stack_ptr);

extern void CALL_CONV coroutine_init_stack(const tmpinfo* info, void** old_stack_ptr);

/* C routines */

void* coroutine_yield(context* ctx, void* datap)
{
	// If this assert triggers then the coroutine stack has overflowed
	// there isn't really any recovery that can be done here. It's already
	// too late. The only solution is to increase the stack size of the 
	// coroutine at creation time. Alternatively platform specific features
	// can be used to build a large stack that doesn't use much actual memory.
	assert(ctx->coroutine.stack_pointer == NULL || VALID_SP(ctx->coroutine.stack_start, ctx->coroutine.stack_pointer));

	ctx->datap = datap;
	coroutine_jmp_stack(ctx->caller.stack_pointer, &ctx->coroutine.stack_pointer);
	return ctx->datap;
}

void CALL_CONV _coroutine_init_func(const tmpinfo* info)
{
	// info stops being valid after we call yield
	// so we have to save ctx now
	context* ctx = info->ctx;
	// Same with funcptr
	void(*funcptr)(void*) = info->funcptr;

	// Yield and get the first value from the caller
	void* datap = coroutine_yield(ctx, NULL);

	// Now that the caller has called next
	// we can execute the coroutine method
	funcptr(datap);

	// Indicate that the coroutine has completed
	ctx->complete = true;
}

char coroutine_is_complete(const context* ctx)
{
	return ctx->complete;
}

void* coroutine_next(context* ctx, void* datap)
{
	if (!coroutine_is_complete(ctx))
	{
		ctx->datap = datap;
		coroutine_jmp_stack(ctx->coroutine.stack_pointer, &ctx->caller.stack_pointer);
	}
	return ctx->datap;
}

context* coroutine_start(coroutine initdata)
{
	void* buffer = malloc(initdata.stack_size);
	context* ctx = coroutine_start_with_mem(initdata, buffer);

	if (ctx != NULL)
		ctx->external_mem = false;
	else if (buffer != NULL)
		// Free buffer if coroutine creation failed
		free(buffer);

	return ctx;
}
context* coroutine_start_with_mem(coroutine initdata, void* stackmem)
{
	// Make sure we got a valid stack pointer
	if (!stackmem || !initdata.funcptr)
		return NULL;

	context* ctx = malloc(sizeof(context));

	ctx->coroutine.stack_start = stackmem;
	ctx->coroutine.stack_pointer = NULL;
	ctx->caller.stack_pointer = NULL;
	ctx->complete = false;
	ctx->external_mem = true;
	ctx->datap = NULL;

	tmpinfo info = {
		// Adjust stack pointer so that the stack grows into
		// the buffer. This needs to happen because the stack
		// can grow in different directions depending on the 
		// CPU that this is running on.
		ADJUST_SP(initdata.stack_size, ctx->coroutine.stack_start),
		&_coroutine_init_func,
		initdata.funcptr,
		ctx
	};

	coroutine_init_stack(&info, &ctx->caller.stack_pointer);

	return ctx;
}
void coroutine_destroy(context* ctx, void* datap)
{
	// Keep executing the coroutine until it is finished
	while (!coroutine_is_complete(ctx))
	{
		coroutine_next(ctx, datap);
	}

	// Free up resources
	coroutine_abort(ctx);
}
void coroutine_abort(context* ctx)
{
	if (!ctx->external_mem)
		free(ctx->coroutine.stack_start);
	free(ctx);
}
