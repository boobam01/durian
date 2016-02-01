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
      return this->generator.render(ctx, "tpl/selectica/GetBOMetaData", "tpl/selectica/root");
    }

    shared_ptr<string> getTrackingNumsMsg(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/GetTrackingNums", "tpl/selectica/root");
    }

    shared_ptr<string> getDataMsg(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/GetData", "tpl/selectica/root");
    }

    shared_ptr<string> createBOMsg(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/CreateBO", "tpl/selectica/root");
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
        response["GetBOTypes"] = raw;
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

    shared_ptr<Plustache::Context> selectica::client<socketType>::GetTrackingNums(char* boType) {

      // Login response
      auto loginResponse = [&](boost::future<std::string> f){
        auto res = f.get();
        auto raw = xml2json(res.c_str());
        // parse the message
        rapidjson::Document d;
        d.Parse(raw.c_str());
        // get the token from the message
        auto tok = d["Envelope"]["Body"]["LoginResponse"]["SessionToken"].GetString();
        // add token to context
        ctx->add("token", tok);
        return string(tok);
      };

      // GetTrackingNums
      auto getTrackingNums = [&](boost::future<std::string> fut){
        // the session token is already stored in the context (ctx)
        ctx->add("boType", boType);
        auto getTrackingNumsXml = this->getTrackingNumsMsg(ctx);
        // future factory
        auto future2 = apiClient->methodPromise["POST"];
        // create the future
        auto f2 = future2(servicePath, *getTrackingNumsXml, customHeaders);
        return f2.get();
      };

      // GetTrackingNums response
      auto getTrackingNumsResponse = [&](boost::future<std::string> fut){
        auto res = fut.get();
        auto raw = xml2json(res.c_str());
        //save response in the context, this will be used by the caller
        response["GetTrackingNums"] = raw;
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
        .then(getTrackingNums)
        .then(getTrackingNumsResponse)
        .then(logout)
        .then(logoutResponse);

      // wait
      auto resp = promises.get();

      return ctx;
    }

    shared_ptr<Plustache::Context> selectica::client<socketType>::GetData(char* trackingNumber) {

      // Login response
      auto loginResponse = [&](boost::future<std::string> f){
        auto res = f.get();
        auto tok = getToken(res);
        ctx->add("token", tok);
        return tok;
      };

      // GetTrackingNums
      auto getData = [&](boost::future<std::string> fut){
        // the session token is already stored in the context (ctx)
        ctx->add("trackingNumber", trackingNumber);
        auto getDataXml = this->getDataMsg(ctx);
        // future factory
        auto future2 = apiClient->methodPromise["POST"];
        // create the future
        auto f2 = future2(servicePath, *getDataXml, customHeaders);
        return f2.get();
      };

      // GetTrackingNums response
      auto getDataResponse = [&](boost::future<std::string> fut){
        auto res = fut.get();
        auto raw = getResponse(res);
        //save response in the context, this will be used by the caller
        ctx->add("GetData", raw);
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
        return f.get();
      };

      // get a login future
      auto login = apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = this->loginMsg(ctx);
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse)
        .then(getData)
        .then(getDataResponse)
        .then(logout)
        .then(logoutResponse);

      // wait
      auto resp = promises.get();

      return ctx;
    }

    shared_ptr<Plustache::Context> selectica::client<socketType>::CreateBO(PlustacheTypes::CollectionType& propertyList) {

      // Login response
      auto loginResponse = [&](boost::future<std::string> f){
        auto res = f.get();
        auto tok = getToken(res);
        ctx->add("token", tok);
        return tok;
      };

      // GetTrackingNums
      auto createBO = [&](boost::future<std::string> fut){
        // the session token is already stored in the context (ctx)
        ctx->add("propertyList", propertyList);
        auto createBOXml = this->createBOMsg(ctx);
        
        dumpFile("CreateBO.xml", createBOXml->c_str());

        // future factory
        auto future2 = apiClient->methodPromise["POST"];
        // create the future
        auto f2 = future2(servicePath, *createBOXml, customHeaders);
        return f2.get();
      };

      // GetTrackingNums response
      auto createBOResponse = [&](boost::future<std::string> fut){
        auto res = fut.get();
        auto raw = getResponse(res);
        //save response in the context, this will be used by the caller
        ctx->add("CreateBO", raw);
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
        return f.get();
      };

      // get a login future
      auto login = apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = this->loginMsg(ctx);
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse)
        .then(createBO)
        .then(createBOResponse)
        .then(logout)
        .then(logoutResponse);

      // wait
      auto resp = promises.get();

      return ctx;
    }

    private:
      static string getToken(string res) {
        auto raw = xml2json(res.c_str());
        // parse the message
        rapidjson::Document d;
        d.Parse(raw.c_str());
        // get the token from the message
        auto tok = d["Envelope"]["Body"]["LoginResponse"]["SessionToken"].GetString();
        return string(tok);
      }

      static string getResponse(string res) {
        auto raw = xml2json(res.c_str());
        return raw;
      }

  };

}

#endif