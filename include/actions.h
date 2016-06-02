#pragma once

#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>
namespace {  
  template <typename CTX, typename FUNC, typename... PARAMS>
  function<CTX(CTX)> createAction(FUNC f_, PARAMS... p) {
    return[=](CTX x)->CTX {
      return f_(x, p...);
    };
  }
}

#endif

/*
// Example:

// Your action function is expected to have following signature:
// (CONTEXT, PARAMS...) => CONTEXT
// The first parameter is a state object that will be passed to your function when your function is finally executed.
// Typically, the state object is a shared_ptr<TYPE>
// The rest of the parameters are what your function will use during execution.  It can be zero or more.
// In your action function, you can interact with the state object.

// createAction has the following signature:
// createAction<YOUR_FUNCTION, VALUES...> => (NEW_FUNCTION(CONTEXT) => CONTEXT)
// createAction takes your action function as its first parameter and the actual values that your function will consume.
// createAction returns a new function that takes in a state object and returns that state object  

// test actions
{
typedef shared_ptr<std::vector<string>> CONTEXT;

auto f = [](CONTEXT v, string t)->CONTEXT {
(*v).push_back(t);
return v;
};

auto f2 = [](CONTEXT v, string t, const string t2)->CONTEXT {
(*v).push_back(t);
(*v).push_back(t2);
return v;
};

auto f3 = [](CONTEXT v, string t, string t2, const string t3)->CONTEXT {
(*v).push_back(t);
(*v).push_back(t2);
(*v).push_back(t3);
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
auto action = createAction(f, param);

// dispatch action with 1 parameter
auto newContext = action(context);

// expect => Hello World
std::copy((*newContext).begin(), (*newContext).end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// test 2 parameter arguments
// returns a function f(context, param...)
(*context).erase((*context).begin() + 1);
auto action2 = createAction(f2, param, param2);

// dispatch action with 2 parameter
auto newContext2 = action2(context);

// expect => Hello World John
ss.str("");
std::copy((*newContext2).begin(), (*newContext2).end(), std::ostream_iterator<std::string>(ss, " "));
cout << ss.str() << endl;

// test 3 parameter arguments
// returns a function f(context, param...)
(*context).erase((*context).begin() + 1);
auto action3 = createAction(f3, param, param2, param3);

// dispatch action with 3 parameter
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