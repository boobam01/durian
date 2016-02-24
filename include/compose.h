#ifndef COMPOSE_H
#define COMPOSE_H

#include <iostream>
#include <tuple>

namespace {
  /*
    Author: http://stackoverflow.com/users/1670129/igor-tandetnik
    Source: http://stackoverflow.com/questions/19071268/function-composition-in-c-c11/27727236#27727236
    http://stackoverflow.com/questions/15904288/how-to-reverse-the-order-of-arguments-of-a-variadic-template-function
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
  class Composer2 <F> {
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
  
  // flow
  // reversed indices...
  template<unsigned... Is> struct seq{ using type = seq; };

  template<unsigned I, unsigned... Is>
  struct rgen_seq : rgen_seq<I - 1, Is..., I - 1>{};

  template<unsigned... Is>
  struct rgen_seq<0, Is...> : seq<Is...>{};

  template<typename CTX>
  struct flow {
    CTX ctx;
    flow(CTX _ctx) { ctx = _ctx; }
    
    template<class Tup, unsigned... Is>
    CTX revertHelper(Tup&& t, seq<Is...>) {
      return compose(std::get<Is>(std::forward<Tup>(t))...)(ctx);
    }

    template <typename... F>
    CTX operator() (F... f) {
      auto t = std::forward_as_tuple(std::forward<F>(f)...);
      return revertHelper(t, rgen_seq<sizeof...(F)>{});
    }
  };

  /*
  int f(int x) { return x + 1; }
  int g(int x) { return x * 2; }
  int h(int x) { return x - 1; }

  int main() {
    // compose
    // f(g(h(42))) right to left
    // result is 83
    std::cout << compose(f, g, h)(42);
    
    // flow
    // h(g(f(42))) left to right
    // result is 85
    flow<int> f1(42);
    std::cout << f1(f, g, h) << endl;

    return 0;
  }
  */

}

#endif