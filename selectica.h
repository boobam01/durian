#ifndef SELECTICA_H
#define SELECTICA_H

#include "api_client.h" /* api_client.h MUST go first b/c ASIO */
#include "durian.h"
#include <functional>

using namespace std;
using namespace durian;

namespace selectica {
  
  /**
    Template generator
  **/
  class client {
    durian::generator<XmlElement::Message> generator;
  public:
    client(){}
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
  };

  /**
    API client with promises
  **/
  template<typename socketType>
  class api {
    
  public:
    api<socketType>(const char* _host,
      const char* _servicePath,
      const char* _user,
      const char* _password,
      std::map<string, string>& _customHeaders) : host(_host), servicePath(_servicePath), user(_user), password(_password), customHeaders(_customHeaders) {

      // create api::client<socketType>
      apiClient = make_unique<api::client<SimpleWeb::HTTP>>(host);

      // create selectica::client for action message generation
      client = make_unique<selectica::client>();

      // store user & password in the context
      user["user"] = _user;
      password["password"] = _password;
      (*ctx).add(user);
      (*ctx).add(password);

    }
    ~api(){}

    string GetBOTypes() {

      // Logout
      auto logout = [&](boost::future<std::string> f){
        auto logoutXml = client.logoutMsg(ctx);
        auto future = apiClient->methodPromise["POST"];
        auto pms = future(servicePath, *logoutXml, customHeaders);
        return pms.get();
      };

      // Logout response
      auto logoutResponse = [&](boost::future<std::string> f){
        auto res = f.get();
        return res;
      };

      // Login response
      auto loginResponse = [&](boost::future<std::string> f){
        auto res = f.get();
        dumpFile("LoginResponse.xml", res.c_str());

        auto raw = xml2json(res.c_str());

        dumpFile("LoginResponse.xml", res.c_str());

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

        auto tok = fut.get();
        //ctx.get()->get("token")
        // getBOTypes -> token should appear in XML b/c context
        auto getBOTypesXml = client.getBOTypesMsg(ctx);

        // test regex
        // captures not working so well in C++ 11
        *getBOTypesXml = std::regex_replace(*getBOTypesXml, sessRgx, "<SessionToken>" + tok + "</SessionToken>");

        dumpFile("GetBOTypes.xml", (*getBOTypesXml).c_str());

        auto future2 = apiClient->methodPromise["POST"];
        auto f2 = future2(servicePath, *getBOTypesXml, customHeaders);

        return f2.get();
      };

      //GetBOTypes response
      auto getBOTypesResponse = [&](boost::future<std::string> fut){
        auto res = fut.get();

        dumpFile("GetBOTypesResponse.xml", res.c_str());

        auto raw = xml2json(res.c_str());
        rapidjson::Document d;
        d.Parse(raw.c_str());

        dumpPrettyJson("GetBOTypesResponse.js", d);

        return raw;
      };

      // get a login future
      auto login = apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = client->loginMsg(ctx);
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse)
        .then(getBOTypes)
        .then(getBOTypesResponse)
        .then(logout)
        .then(logoutResponse);

      auto resp =  promises.get();

      cout << resp << "\n";

      return resp;
    }

    /**
      // Experimental
    
    typedef std::function<string(string, string)> Funcp = func<string>;
    template<typename T>
    string chainPromises(Funcp f0, Funcp f1, Funcp f2) {

      return "";
    }
    **/
    
  private:
    unique_ptr<api::client<socketType>> apiClient;
    unique_ptr<selectica::client> client;
    PlustacheTypes::ObjectType user, password, token;
    shared_ptr<Plustache::Context> ctx;
    std::map<string, string> customHeaders;
    string host, servicePath;
    
  };

}

#endif