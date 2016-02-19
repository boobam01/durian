#ifndef COMPOSE_H
#define COMPOSE_H

#include <iostream>

namespace {
  /*
    Author: http://stackoverflow.com/users/1670129/igor-tandetnik
    Source: http://stackoverflow.com/questions/19071268/function-composition-in-c-c11/27727236#27727236
    */
  template <typename F0, typename... F>
  class Composer2 {
    F0 f0_;
    Composer2<F...> tail_;
  public:
    Composer2(F0 f0, F... f) : f0_(f0), tail_(f...) {}

    template <typename T>
    T operator() (const T& x) const {
      return f0_(tail_(x));
    }
  };

  template <typename F>
  class Composer2 < F > {
    F f_;
  public:
    Composer2(F f) : f_(f) {}

    template <typename T>
    T operator() (const T& x) const {
      return f_(x);
    }
  };

  template <typename... F>
  Composer2<F...> compose(F... f) {
    return Composer2<F...>(f...);
  }

}
/*
int f(int x) { return x + 1; }
int g(int x) { return x * 2; }
int h(int x) { return x - 1; }

int main() {
  std::cout << compose(f, g, h)(42);
  return 0;
}
*/

#endif