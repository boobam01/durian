#pragma once

#ifndef COMPOSE_H
#define COMPOSE_H

#include <iostream>

namespace
{
  /*
    Author: http://stackoverflow.com/users/1670129/igor-tandetnik
    Source: http://stackoverflow.com/questions/19071268/function-composition-in-c-c11/27727236#27727236
    http://stackoverflow.com/questions/15904288/how-to-reverse-the-order-of-arguments-of-a-variadic-template-function
    */

	using namespace std;

	template <typename F0, typename... F>
	class Composer {
		F0 f0_;
		Composer<F...> tail_;
	public:
		Composer(F0 f0, F... f) : f0_(f0), tail_(f...) {}

		template <typename T>
		T operator() (const T& x) const {
			return f0_(tail_(x));
		}
	};

	template <typename F>
	class Composer <F> {
		F f_;
	public:
		Composer(F f) : f_(f) {}

		template <typename T>
		T operator() (const T& x) const {
			return f_(x);
		}
	};

	template <typename... F>
	Composer<F...> compose(F... f) {
		return Composer<F...>(f...);
	}

	template <typename F0, typename... F>
	class Flow {
		F0 f0_;
		Flow<F...> tail_;
	public:
		Flow(F0 f0, F... f) : f0_(f0), tail_(f...) {}

		template <typename T>
		T operator() (const T& x) const {
			return tail_(f0_(x));
		}
	};

	template <typename F>
	class Flow <F> {
		F f_;
	public:
		Flow(F f) : f_(f) {}

		template <typename T>
		T operator() (const T& x) const {
			return f_(x);
		}
	};

	template <typename... F>
	Flow<F...> flow(F... f) {
		return Flow<F...>(f...);
	}

	// int f(int x) { return x + 1; }
	// int g(int x) { return x * 2; }
	// int h(int x) { return x - 1; }

	// int main()
	// {
		// cout << "compose(f, g, h)(42) => f(g(h(42)))" << endl;
		// cout << "expected: 83" << endl;
		// cout << compose(f, g, h)(42) << endl;

		// cout << "flow(f, g, h)(42) => h(g(f(42)))" << endl;
		// cout << "expected: 85" << endl;
		// cout << "result: " << flow(f, g, h)(42) << endl;
		
		// return 0;
	// }
}

#endif