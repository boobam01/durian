#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>
namespace {
  
  template<typename FUNC, typename CONTEXT, typename T>
  class Composer {
    FUNC f_;
    CONTEXT c_;
    T t_;
  public:
    Composer(FUNC f, CONTEXT c, T t) : f_(f), c_(c), t_(t) {}

    template <typename CTX, typename T>
    CTX operator() (CTX& x, T& t) const {
      return f_(x, t);
    }
  };

  template <typename F, typename C, typename T>
  Composer<F,C,T> createAction(const F f, C c, T t) {
    return Composer<F,C,T>(f, c, t);
  }

}

#endif