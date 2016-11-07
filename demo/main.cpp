#include "coroutine_wrapper.h"

#include <iostream>

void func(coroutine& c)
{
	for (size_t i = 0; i < 100; ++i)
		c.yield(i);
}

int main(int argc, char** argv)
{
	coroutine c{ 1024 * 1024, &func };

	while (!c.complete())
	{
		std::cout << c.next();
	}
}
