#include "generator.h"

#include <iostream>
#include <vector>

void func(size_t start, size_t stop, size_t step)
{
	for (size_t i = start; i < stop; i += step)
		yield(i);
}

template<typename T>
gen_wrapper<T> range(T start, T stop, T step = 1)
{
	return make_generator<T>([=]
	{
		for (size_t i = start; i < stop; i += step)
			yield(i);
	}, 1024 * 1024);
}

int main(int argc, char** argv)
{
	auto gen = range<size_t>(0, 100, 1).gen;
	if (!gen->complete())
	{
		do
		{
			std::cout << (gen->value()) << '\n';
		} while (gen->next());
	}

	std::cout << std::endl;

}
