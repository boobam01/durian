#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>
namespace {
  
  template <typename F, typename CTX, typename... PARAMS>
  std::function<CTX(CTX)> createAction(const F f_, CTX& x, PARAMS... p) {
    return [=, &x](CTX&)->CTX {
      f_(x, &p...);
      return x;
    };
  }

}

#endif

/*
// Example:

// test thunks
{
typedef shared_ptr<std::vector<string>> CONTEXT;

auto f = [](CONTEXT v, const string* t)->CONTEXT {
(*v).push_back(*t);
return v;
};

auto f2 = [](CONTEXT v, const string* t, const string* t2)->CONTEXT {
(*v).push_back(*t);
(*v).push_back(*t2);
return v;
};

auto f3 = [](CONTEXT v, const string* t, const string* t2, const string* t3)->CONTEXT {
(*v).push_back(*t);
(*v).push_back(*t2);
(*v).push_back(*t3);
return v;
};

CONTEXT context = make_shared<std::vector<string>>();
*context = { "Hello" };
string param("World");
string param2("John");
string param3("Smith");
std::ostringstream ss;

// test 1 parameter argument
// returns a function f(context, param...)
auto action = createAction(f, context, param);

// dispatch action with 1 parameter
auto newContext = action(context);

// auto newContext = action(context);

// expect => Hello World
std::copy((*newContext).begin(), (*newContext).end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// test 2 parameter arguments
// returns a function f(context, param...)
(*context).erase((*context).begin() + 1);
auto action2 = createAction(f2, context, param, param2);

// dispatch action with 2 parameter
auto newContext2 = action2(context);

// expect => Hello World John
ss.str("");
std::copy((*newContext2).begin(), (*newContext2).end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// test 3 parameter arguments
// returns a function f(context, param...)
(*context).erase((*context).begin() + 1);
auto action3 = createAction(f3, context, param, param2, param3);

// dispatch action with 2 parameter
auto newContext3 = action3(context);

// expect => Hello World John Smith
ss.str("");
std::copy((*newContext3).begin(), (*newContext3).end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// console should display:
// Hello World
// Hello World John
// Hello World John Smith
}
*/