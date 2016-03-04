#ifndef API_SERVER_H
#define API_SERVER_H

#include "server_http.hpp"
#include "server_https.hpp"
#include <type_traits>
#include <tuple>

using namespace std;

namespace api {

  template<typename T>
  struct Route {
    string expression;
    string verb;
    std::function<void(typename SimpleWeb::ServerBase<T>::Reponse&, shared_ptr<typename SimpleWeb::ServerBase<T>::Request>)> action;
  };

  template<typename T>
  class server {
  public:
    server<T>(int _port, int _threads, vector<Route<T>>& routes) : port(_port), threads(_threads) {
      _server = make_shared<SimpleWeb::Server<T>>(port, threads);
      connect(routes);
    }
    server<T>(int _port, int _threads, const string _certFile, const string _privateKeyFile) : port(_port), threads(_threads), certFile(_certFile), privateKeyFile(_privateKeyFile) {
      if (std::is_same<T, SimpleWeb::HTTP>) {
        _server = make_shared<SimpleWeb::Server<T>>(port, threads);
      }
      else {
        _server = make_shared<SimpleWeb::Server<T>>(port, threads, sertFile, privateKeyFile);
      }
      connect(routes);
    }
    //Start server
    void start() {
      std::thread server_thread([&(*_server)](){
        (*_server).start();
      });
      //Wait for server to start so that the client can connect
      std::this_thread::sleep_for(chrono::seconds(1));
    }
  private:
    int port, threads;
    std::string certFile, privateKeyFile;
    shared_ptr<SimpleWeb::Server<T>> _server;
    void connect(vector<Route<T>>& routes) {
      std::for_each(routes.begin(), routes.end(), [](const Route& r){
        (*_server)->resource[r.expression][r.verb] = r.action;
      });
      //default
      (*_server)->default_resource["GET"] = [](SimpleWeb::ServerBase<T>::Reponse& response, shared_ptr<SimpleWeb::ServerBase<T>::Request>> request) {
        response << "HTTP/1.1 200 OK\r\nContent-Length: " << 0 << "\r\n\r\n";
      };
    }
  };
}

#endif