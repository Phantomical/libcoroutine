#pragma once

#include <tuple>
#include <functional>
#include <cassert>
#include <memory>

template<typename T>
class generator;
typedef struct _context context;


namespace impl
{
	template<typename T>
	thread_local generator<T>* _generator;

	void* yield(context* ctx, void* ptr);
	void* next(context* ctx, void* ptr);
	bool is_complete(const context* ctx);

	context* start(void(*func)(void*), size_t stack_size);
	context* start(void(*func)(void*), size_t stack_size, void* stackmem);
}

template<typename T>
class generator : std::enable_shared_from_this<generator<T>>
{
private:
	struct init_args
	{
		generator* gen;
		std::function<void()> func;
	};

	context* ctx;
	const T* ptr;

	static void coroutine_base(void* ptr)
	{
		init_args* args = (init_args*)ptr;
		auto gen = args->gen;
		auto fn = args->func;

		args->gen->yield(T());

		impl::_generator<T> = gen;

		fn();
	}

public:
	class iterator
	{
	public:
		typedef T value_type;
		typedef const T& reference;
		typedef const T* pointer;
		typedef std::output_iterator_tag iterator_category;

	private:
		std::shared_ptr<generator<T>> ptr;
		bool complete;

	public:
		iterator& operator++()
		{
			complete = ptr->next();

			return *this;
		}

		const T& operator*() const
		{
			return ptr->value();
		}

		bool operator ==(const iterator& it) const
		{
			return it.complete == complete;
		}
		bool operator !=(const iterator& it) const
		{
			return !(*this == it);
		}

		iterator(std::shared_ptr<generator<T>> ptr, bool complete = false) :
			ptr(ptr),
			complete(complete)
		{

		}
	};

	void yield(const T& val)
	{
		T* ptr = const_cast<T*>(&val);

		auto gen = impl::_generator<T>;
		impl::_generator<T> = nullptr;
		impl::yield(ctx, ptr);
		impl::_generator<T> = gen;
	}
	bool next()
	{
		auto gen = impl::_generator<T>;
		ptr = (const T*)impl::next(ctx, nullptr);
		impl::_generator<T> = gen;
		return !complete();
	}
	const T& value() const
	{
		return *ptr;
	}

	bool complete() const
	{
		return impl::is_complete(ctx);
	}
	
	generator(const std::function<void()>& func, size_t stack_size) :
		ptr(nullptr)
	{
		init_args args = {
			this,
			func
		};
		ctx = impl::start(&coroutine_base, stack_size);

		impl::next(ctx, &args);
		next();
	}
	generator(const std::function<void()>& func, size_t stack_size, void* stackmem) :
		ptr(nullptr)
	{
		init_args args = {
			this,
			func
		};
		ctx = impl::start(&coroutine_base, stack_size, stackmem);

		impl::next(ctx, &args);
		next();
	}
};

template<typename T>
struct gen_wrapper
{
	std::shared_ptr<generator<T>> gen;

	typename generator<T>::iterator begin()
	{
		return typename generator<T>::iterator{ gen };
	}
	typename generator<T>::iterator end()
	{
		return typename generator<T>::iterator{ gen, true };
	}
};

template<typename T>
gen_wrapper<T> make_generator(const std::function<void()>& func, size_t stack_size = 1024 * 128)
{
	return{ std::make_shared<generator<T>>(func, stack_size) };
}

#ifndef GENERATOR_IMPL
template<typename T>
void yield(const T& val)
{
	// Check to make sure that we are executing a coroutine
	assert(impl::_generator<T> != nullptr);

	impl::_generator<T>->yield(val);
}
#endif
