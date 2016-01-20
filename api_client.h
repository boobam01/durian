#ifndef API_CLIENT_H
#define	API_CLIENT_H

#ifndef BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE
#endif

#ifndef BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#endif 

#ifndef BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#endif

#include "client_http.hpp"
#include "client_https.hpp"

#include<boost/thread/future.hpp>
#include <iostream>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <map>

using namespace std;

static const std::list<string> methods{ "GET", "POST", "PUT", "PATCH", "DEL" };

namespace api {

  template<class T>
  class client {
  public:
    client(string _host) : host(_host) {

      std::for_each(methods.begin(), methods.end(), [this](const string verb)->void{

        // promise pattern
        this->methodPromise[verb] = [verb, this](string& params, string& data, std::map<string, string>& header)->boost::future<std::string> {

          auto apiCall = [](const string host, const string verb, string& params, string& data, std::map<string, string>& header)->string {

            SimpleWeb::Client<T> client(host);

            shared_ptr<SimpleWeb::ClientBase<T>::Response> r1;

            if (data.size() > 0){
              stringstream ss;
              ss << data;
              r1 = client.request(verb, params, ss, header);
            }
            else {
              r1 = client.request(verb, params, "", header);
            }
            ostringstream oss;
            oss << r1->content.rdbuf();
            return oss.str();
          };

          return boost::async([&]()->string{
            return apiCall(host, verb, params, data, header);
          });

        };
      });
    }
    ~client(){}
    /*
    map of <key, fn(string, string) => future<string>>
    */
    std::unordered_map <string, std::function<boost::future<std::string>(string&, string&, std::map<string, string>&)>> methodPromise;

  private:
    string host;
  };

}

#endif