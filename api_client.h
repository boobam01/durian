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
#include "semaphore.hpp"
// #include "plustache/plustache_types.hpp"
// #include "plustache/context.hpp"

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
  
  struct IOIO {
    IOIO(string& _method, string& _host, string& _path, string& _data, std::map<string, string>& _headers)
      : method(_method), host(_host), path(_path), data(_data), headers(_headers) {}
    IOIO(const char* _method, const char* _host, const char* _path, const char* _data, std::map<string, string>& _headers)
      : method(_method), host(_host), path(_path), data(_data), headers(_headers) {}
    string method;
    string host;
    string path;
    string data;
    string response;
    std::map<string, string> headers;
  };

  template<typename T>
  class client {
  public:
    client(string _host) : host(_host) {

      // for each http/s verb, create a method factory that returns a future
      std::for_each(methods.begin(), methods.end(), [this](const string verb)->void{

        // promise pattern
        this->methodPromise[verb] = [verb, this](string& params, string& data, std::map<string, string>& header)->boost::future <std::string> {
          /*
            implementation of generic REST client
          */
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
        
      }); // le fin for_each
    }
    client(){};
    ~client(){}
    /*
    map of <key, fn(string, string) => future<string>>
    */
    std::unordered_map <string, std::function<boost::future<std::string>(string&, string&, std::map<string, string>&)>> methodPromise;
    
  private:
    string host;
        
  };

  template<typename T>
  class parallelClientImpl {
  public:
    parallelClientImpl(){}
    ~parallelClientImpl(){}
  };

  // children must implement
  template<typename T>
  class parallelClient : public parallelClientImpl<T>{
  
  public:
    parallelClient(int maxThreads) {
      THREAD_LIMITER = make_unique<::Semaphore>(maxThreads);      
    
      //factory pattern
      this->methodPromise = [&](vector<shared_ptr<IOIO>> futures)->vector<shared_ptr<IOIO>> {
        auto apiCall = [&](shared_ptr<IOIO> o) {
          Semaphore_waiter_notifier w(*THREAD_LIMITER);

          SimpleWeb::Client<T> client(o->host);
          auto resp = client.request(o->method, o->path, o->data, o->headers);
          stringstream ss;
          ss << resp->content.rdbuf();
          o->response = ss.str();
        };

        ::parallel_for_each(futures.begin(), futures.end(), apiCall);

        return futures;
      };
    }
    std::function<std::vector<shared_ptr<IOIO>>(std::vector<shared_ptr<IOIO>>)> methodPromise;
  private:
    unique_ptr<::Semaphore> THREAD_LIMITER;
  };

}

#endif