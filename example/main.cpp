#include "generator.h"

#include <iostream>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

void* alloc_stack(size_t size)
{
	LPVOID mem = VirtualAlloc(nullptr, size + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	VirtualAlloc((char*)mem + size, 4096, MEM_DECOMMIT, PAGE_NOACCESS);
	return mem;
}

void func(size_t start, size_t stop, size_t step)
{
	for (size_t i = start; i < stop; i += step)
		yield(i);
}

template<typename T>
gen_wrapper<T> range(T start, T stop, T step = 1)
{
	static constexpr size_t stack_size = 4096;
	return make_generator<T>([=]
	{
		for (size_t i = start; i < stop; i += step)
			yield(i);
	}, stack_size, alloc_stack(stack_size));
}

int main(int argc, char** argv)
{
	for (auto val : range<size_t>(0, std::numeric_limits<size_t>::max()))
	{
		std::cout << val << '\n';
	}

	std::cout << std::endl;

}
