#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>
namespace {
  
  template<typename FUNC>
  struct Composer {
    FUNC f_;
    Composer(FUNC f) : f_(f) {}

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
  template <typename F>
  Composer<F> createAction(const F f) {
    return Composer<F>(f);
  }

}

#endif

/*
// Example:

// test thunks
{
typedef std::vector<string> CONTEXT;

auto f = [](CONTEXT v, string* t)->CONTEXT {
v.push_back(*t);
return v;
};

auto f2 = [](CONTEXT v, string* t, string* t2)->CONTEXT {
v.push_back(*t);
v.push_back(*t2);
return v;
};

auto f3 = [](CONTEXT v, string* t, string* t2, string* t3)->CONTEXT {
v.push_back(*t);
v.push_back(*t2);
v.push_back(*t3);
return v;
};

CONTEXT context{ "Hello" };
string param("World");
string param2("John");
string param3("Smith");
std::ostringstream ss;

// test 1 parameter argument
// returns a function f(context, param...)
auto action = createAction(f);

// dispatch action with 1 parameter
auto newContext = action(context, param);

// expect => Hello World
std::copy(newContext.begin(), newContext.end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// test 2 parameter arguments
// returns a function f(context, param...)
auto action2 = createAction(f2);

// dispatch action with 2 parameter
auto newContext2 = action2(context, param, param2);

// expect => Hello World John
ss.str("");
std::copy(newContext2.begin(), newContext2.end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// test 3 parameter arguments
// returns a function f(context, param...)
auto action3 = createAction(f3);

// dispatch action with 2 parameter
auto newContext3 = action3(context, param, param2, param3);

// expect => Hello World John Smith
ss.str("");
std::copy(newContext3.begin(), newContext3.end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// console should display:
// Hello World
// Hello World John
// Hello World John Smith
}
*/