#ifndef SELECTICA_H
#define SELECTICA_H

#include "api_client.h" /* api_client.h MUST go first b/c ASIO */
#include "durian.h"
#include <functional>

using namespace std;
using namespace durian;

namespace selectica {
  
  template<typename socketType>
  class client {
    durian::generator<XmlElement::Message> generator;
    unique_ptr<api::client<socketType>> apiClient;
    PlustacheTypes::ObjectType user, password, token, response;
    shared_ptr<Plustache::Context> ctx;
    std::map<string, string> customHeaders;
    string host, servicePath, userstr, passwordstr;    
  public:
    client(){}
    client <socketType>(const char* _host,
      const char* _servicePath,
      const char* _user,
      const char* _password,
      std::map<string, string>& _customHeaders) : host(_host), servicePath(_servicePath), userstr(_user), passwordstr(_password), customHeaders(_customHeaders) {

      // create api::client<socketType>
      apiClient = make_unique<api::client<SimpleWeb::HTTP>>(host);

      // create the context that will be shared by all templates
      ctx = make_shared<Plustache::Context>();

      // store user & password in the context
      user["user"] = userstr;
      password["password"] = passwordstr;
      (*ctx).add(user);
      (*ctx).add(password);
    }
    ~client(){}

    shared_ptr<string> loginMsg(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/Login", "tpl/selectica/root");
    }

    shared_ptr<string> logoutMsg(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/Logout", "tpl/selectica/root");
    }

    shared_ptr<string> getBOTypesMsg(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/GetBOTypes", "tpl/selectica/root");
    }

    shared_ptr<string> getBOMetaDataMsg(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/getBOMetaData", "tpl/selectica/root");
    }

    shared_ptr<Plustache::Context> selectica::client<socketType>::GetBOTypes() {

      // Login response
      auto loginResponse = [&](boost::future<std::string> f){
        auto res = f.get();        
        // xml => json
        auto raw = xml2json(res.c_str());
        // parse the message
        rapidjson::Document d;
        d.Parse(raw.c_str());
        dumpPrettyJson("LoginResponse.js", d);
        // get the token from the message
        auto tok = d["Envelope"]["Body"]["LoginResponse"]["SessionToken"].GetString();
        // add token to context
        token["token"] = tok;
        (*ctx).add(token);

        return string(tok);
      };

      // GetBOTypes
      auto getBOTypes = [&](boost::future<std::string> fut){

        // the session token is already stored in the context (ctx)
        auto getBOTypesXml = this->getBOTypesMsg(ctx);        
        // future factory
        auto future2 = apiClient->methodPromise["POST"];        
        // create the future
        auto f2 = future2(servicePath, *getBOTypesXml, customHeaders);

        return f2.get();
      };

      //GetBOTypes response
      auto getBOTypesResponse = [&](boost::future<std::string> fut){
        
        auto res = fut.get();
        auto raw = xml2json(res.c_str());
        //save response in the context, this will be used by the caller
        response["response"] = raw;
        (*ctx).add(response);

        return raw;
      };

      // Logout
      auto logout = [&](boost::future<std::string> f){
        // the session token is in the ctx, it will be sent to the server to end session
        auto logoutXml = this->logoutMsg(ctx);
        auto future = apiClient->methodPromise["POST"];
        auto pms = future(servicePath, *logoutXml, customHeaders);
        return pms.get();
      };

      // Logout response
      auto logoutResponse = [&](boost::future<std::string> f){
        auto res = f.get();
        return res;
      };

      // get a login future
      auto login = apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = this->loginMsg(ctx);
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse)
        .then(getBOTypes)
        .then(getBOTypesResponse)
        .then(logout)
        .then(logoutResponse);

      // wait
      auto resp = promises.get();

      return ctx;
    }
  };

}

#endif