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

/*
// test thunks
{
auto f = [](string s, string* t)->string {
s.append(*t);
return s;
};

auto f2 = [](string s, string* t, string* t2)->string {
s.append(*t);
s.append(*t2);
return s;
};

auto f3 = [](string s, string* t, string* t2, string* t3)->string {
s.append(*t);
s.append(*t2);
s.append(*t3);
return s;
};

string context("Hello");
string param(" World");
string param2(" John");
string param3(" Smith");

// test 1 parameter argument
// returns a function f(context, param)
auto action = createAction(f, context);

// dispatch action with 1 parameter
auto resp = action(context, param);

// expect => Hello World
cout << resp << endl;

// test 2 parameter arguments
// returns a function f(context, param, param2)
auto action2 = createAction(f2, context);

// dispatch action with 2 parameter
auto resp2 = action2(context, param, param2);

// expect => Hello World John
cout << resp2 << endl;

// test 3 parameter arguments
// returns a function f(context, param, param2, param3)
auto action3 = createAction(f3, context);

// dispatch action with 2 parameter
auto resp3 = action3(context, param, param2, param3);

// expect => Hello World John Smith
cout << resp3 << endl;

// console should display:
// Hello World
// Hello World John
// Hello World John Smith
  }
*/