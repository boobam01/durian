#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>
namespace {
  
  template<typename FUNC, typename CONTEXT>
  class Composer {
    FUNC f_;
    CONTEXT c_;
  public:
    Composer(FUNC f, CONTEXT c) : f_(f), c_(c) {}

    template <typename CTX, typename... PARAMS>
    CTX operator() (CTX& x, PARAMS... p) const {
      return f_(x, &p...);
    }
  };

  /*
    given parameters of a function, a context, and one parameter
    use Variadic templates
    returns a function of func(context, param...)->context
  */
  template <typename F, typename C>
  Composer<F,C> createAction(const F f, C c) {
    return Composer<F,C>(f, c);
  }

}

#endif