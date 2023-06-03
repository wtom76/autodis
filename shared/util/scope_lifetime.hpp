#pragma once

#include "framework.hpp"

namespace shared::util
{
	//---------------------------------------------------------------------------------------------------------
	template <typename data>
	class scope_lifetime
	{
	public:
		using func_t = std::function<void(data&)>;
	private:
		data	data_;
		func_t	dtor_func_;

	public:
		scope_lifetime(func_t&& ctor_func, func_t&& dtor_func)
			: dtor_func_{std::move(dtor_func)}
		{
			ctor_func(data_);
		}
		~scope_lifetime()
		{
			dtor_func_(data_);
		}
	};
	//---------------------------------------------------------------------------------------------------------
	template <>
	class scope_lifetime<void>
	{
	public:
		using func_t = std::function<void()>;
	private:
		func_t	dtor_func_;

	public:
		scope_lifetime(func_t&& ctor_func, func_t&& dtor_func)
			: dtor_func_{std::move(dtor_func)}
		{
			ctor_func();
		}
		~scope_lifetime()
		{
			dtor_func_();
		}
	};
}