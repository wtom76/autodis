#pragma once

#include <cmath>

namespace learning::activation
{
	//-----------------------------------------------------------------------------------------------------
	// abstract class Function
	//-----------------------------------------------------------------------------------------------------
	class function
	{
	public:
		//-----------------------------------------------------------------------------------------------------
		virtual double activation(double x) const = 0;
		//-----------------------------------------------------------------------------------------------------
		virtual double derivative(double x) const = 0;
	};
	//-----------------------------------------------------------------------------------------------------
	// class sigma
	//-----------------------------------------------------------------------------------------------------
	class sigma : public function
	{
	public:
		//-----------------------------------------------------------------------------------------------------
		double activation(double x) const override
		{
			return 1. / (1. + std::exp(-x));
		}
		//-----------------------------------------------------------------------------------------------------
		double derivative(double x) const override
		{
			return (1. - x) * x;
		}
	};
	//-----------------------------------------------------------------------------------------------------
	// Hyperbolic tangent
	// class Tanh
	//-----------------------------------------------------------------------------------------------------
	class tanh : public function
	{
	public:
		//-----------------------------------------------------------------------------------------------------
		double activation(double x) const override
		{
			return 2. / (1. + std::exp(-2. * x)) - 1.;
		}
		//-----------------------------------------------------------------------------------------------------
		double derivative(double x) const override
		{
			const double act = activation(x);
			return 1. - act * act;
		}
	};
	//-----------------------------------------------------------------------------------------------------
	// Gaussian
	// class Gaussian
	//-----------------------------------------------------------------------------------------------------
	class gaussian : public function
	{
	public:
		//-----------------------------------------------------------------------------------------------------
		double activation(double x) const override
		{
			return std::exp(-(x * x));
		}
		//-----------------------------------------------------------------------------------------------------
		double derivative(double x) const override
		{
			return -2. * x * std::exp(-(x * x));
		}
	};
	//-----------------------------------------------------------------------------------------------------
	// Identity
	// class Identity
	//-----------------------------------------------------------------------------------------------------
	class identity : public function
	{
	public:
		//-----------------------------------------------------------------------------------------------------
		double activation(double net) const override
		{
			return net;
		}
		//-----------------------------------------------------------------------------------------------------
		double derivative(double) const override
		{
			return 1.;
		}
	};
}