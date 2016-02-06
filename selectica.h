#ifndef SELECTICA_H
#define SELECTICA_H

#include "api_client.h" /* api_client.h MUST go first b/c ASIO */
#include "durian.h"
#include <functional>

using namespace std;
using namespace durian;

namespace selectica {

  template<typename socketType>
  class client : public durian::client<socketType> {
  public:  
    client (const char* _host,
      const char* _servicePath,
      const char* _user,
      const char* _password,
      std::map<string, string>& _customHeaders) : durian::client<socketType>(_host, _servicePath, _user, _password, _customHeaders) {}
  
    shared_ptr<Plustache::Context> selectica::client<socketType>::GetBOTypes() {
      // get a login future
      auto login = this->apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = this->generator.render(ctx, "tpl/selectica/Login", "tpl/selectica/root");
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse(this->ctx))
        .then(getBOTypes(this->ctx, "tpl/selectica/GetBOTypes", "tpl/selectica/root", trackingNumber))
        .then(getBOTypesResponse())
        .then(logout(this->ctx, "tpl/selectica/Logout", "tpl/selectica/root"))
        .then(logoutResponse());
      //wait
      auto resp = promises.get();

      return ctx;
    }

    shared_ptr<Plustache::Context> selectica::client<socketType>::GetTrackingNums(const string boType) {
      // get a login future
      auto login = this->apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = this->generator.render(ctx, "tpl/selectica/Login", "tpl/selectica/root");
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse(this->ctx))
        .then(getTrackingNums(this->ctx, "tpl/selectica/GetTrackingNums", "tpl/selectica/root", boType))
        .then(getTrackingNumsResponse())
        .then(logout(this->ctx, "tpl/selectica/Logout", "tpl/selectica/root"))
        .then(logoutResponse());
      //wait
      auto resp = promises.get();

      return ctx;
    }

    shared_ptr<Plustache::Context> selectica::client<socketType>::GetData(const string trackingNumber) {
      // get a login future
      auto login = this->apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = this->generator.render(ctx, "tpl/selectica/Login", "tpl/selectica/root");       
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse(this->ctx))
        .then(getData(this->ctx, "tpl/selectica/GetData", "tpl/selectica/root", trackingNumber))
        .then(getDataResponse())
        .then(logout(this->ctx, "tpl/selectica/Logout", "tpl/selectica/root"))
        .then(logoutResponse());
      //wait
      auto resp = promises.get();

      return ctx;
    }

    shared_ptr<Plustache::Context> selectica::client<socketType>::CreateBO(const string boType, const string listType, PlustacheTypes::CollectionType& property) {
      // get a login future
      auto login = this->apiClient->methodPromise["POST"];
      // get login template
      auto loginXml = this->generator.render(ctx, "tpl/selectica/Login", "tpl/selectica/root");
      // start the promises
      auto promises = login(servicePath, *loginXml, customHeaders)
        .then(loginResponse(this->ctx))
        .then(createBO(this->ctx, "tpl/selectica/CreateBO", "tpl/selectica/root", boType, listType, property))
        .then(createBOResponse())
        .then(logout(this->ctx, "tpl/selectica/Logout", "tpl/selectica/root"))
        .then(logoutResponse());
      //wait
      auto resp = promises.get();

      return ctx;
    }

    private:
      // GetData
      std::function<string(boost::future<std::string>)> getData(shared_ptr<Plustache::Context> ctx, const string bodyTpl, const string rootTpl, const string trackingNumber) {
        return [&, bodyTpl, rootTpl, trackingNumber](boost::future<std::string> f)->string{
          // the session token is already stored in the context (ctx)
          ctx->add("trackingNumber", trackingNumber);
          auto getDataXml = this->generator.render(ctx, bodyTpl, rootTpl);
          // future factory
          auto future2 = apiClient->methodPromise["POST"];
          // create the future
          auto f2 = future2(servicePath, *getDataXml, customHeaders);
          return f2.get();
        };
      }

      // GetData response
      std::function<string(boost::future<std::string>)> getDataResponse() {
        return [&](boost::future<std::string> fut)->string{
          auto res = fut.get();
          auto raw = getResponse(res);
          //save response in the context, this will be used by the caller
          ctx->add("GetData", raw);
          return raw;
        };
      }
      
      // GetTrackingNums
      std::function<string(boost::future<std::string>)> getTrackingNums(shared_ptr<Plustache::Context> ctx, const string bodyTpl, const string rootTpl, const string boType) {
        return [&, bodyTpl, rootTpl, boType](boost::future<std::string> f)->string{
          //auto getTrackingNums = [&](boost::future<std::string> fut){
          // the session token is already stored in the context (ctx)
          ctx->add("boType", boType);
          auto getTrackingNumsXml = this->generator.render(ctx, bodyTpl, rootTpl);
          // future factory
          auto future2 = apiClient->methodPromise["POST"];
          // create the future
          auto f2 = future2(servicePath, *getTrackingNumsXml, customHeaders);
          return f2.get();
        };
      }

      // GetTrackingNums response
      std::function<string(boost::future<std::string>)> getTrackingNumsResponse() {
        return [&](boost::future<std::string> fut)->string{          
          auto res = fut.get();
          auto raw = xml2json(res.c_str());
          //save response in the context, this will be used by the caller
          response["GetTrackingNums"] = raw;
          (*ctx).add(response);
          return raw;
        };
      }

      // GetBOTypes
      std::function<string(boost::future<std::string>)> getBOTypes(shared_ptr<Plustache::Context> ctx, const string bodyTpl, const string rootTpl) {
        return [&, bodyTpl, rootTpl](boost::future<std::string> fut)->string{
          // the session token is already stored in the context (ctx)
          auto getBOTypesXml = this->generator.render(ctx, bodyTpl, rootTpl);
          // future factory
          auto future2 = apiClient->methodPromise["POST"];
          // create the future
          auto f2 = future2(servicePath, *getBOTypesXml, customHeaders);

          return f2.get();
        };
      }

      //GetBOTypes response
      std::function<string(boost::future<std::string>)> getBOTypesResponse() {
        return [&](boost::future<std::string> fut)->string{
          auto res = fut.get();
          auto raw = xml2json(res.c_str());
          //save response in the context, this will be used by the caller
          response["GetBOTypes"] = raw;
          (*ctx).add(response);

          return raw;
        };
      }

      // CreateBO
      std::function<string(boost::future<std::string>)> createBO(shared_ptr<Plustache::Context> ctx, const string bodyTpl, const string rootTpl, const string boType, const string listType, PlustacheTypes::CollectionType& property) {
        return [&, bodyTpl, rootTpl, boType, listType](boost::future<std::string> f)->string{

          //auto createBO = [&](boost::future<std::string> fut){
          // the session token is already stored in the context (ctx)
          ctx->add("boType", boType);
          ctx->add("listType", listType);
          ctx->add("property", property);

          auto createBOXml = this->generator.render(ctx, bodyTpl, rootTpl);

          dumpFile("CreateBO.xml", createBOXml->c_str());

          // future factory
          auto future2 = apiClient->methodPromise["POST"];
          // create the future
          auto f2 = future2(servicePath, *createBOXml, customHeaders);
          return f2.get();
        };
      }

      // CreateBO response
      std::function<string(boost::future<std::string>)> createBOResponse() {
        return [&](boost::future<std::string> fut)->string{
          //auto createBOResponse = [&](boost::future<std::string> fut){
          auto res = fut.get();
          auto raw = getResponse(res);
          //save response in the context, this will be used by the caller
          ctx->add("CreateBO", raw);
          return raw;
        };
      }
  };

}

#endif