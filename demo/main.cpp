#include "coroutine_wrapper.h"

#include <iostream>
#include <vector>

void func(coroutine_wrapper& c)
{
	size_t v = 1;
	for (size_t i = 1; i < 15; ++i)
		c.yield(v *= i);
}

int main(int argc, char** argv)
{
	void* mem = std::malloc(1024 * 1024);
	coroutine_wrapper c{ 1024 * 1024, &func, mem };

	std::vector<size_t> vals;

	while (c.next())
	{
		vals.push_back(c.value());
	}

	for (auto v : vals)
		std::cout << v << '\n';
	std::cout << std::endl;

	free(mem);
}
