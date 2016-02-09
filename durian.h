#ifndef DURIAN_H
#define DURIAN_H

#include <iostream>
#include "spdlog/spdlog.h"
#include "plustache/template.hpp"
#include "plustache/plustache_types.hpp"
#include "plustache/context.hpp"
#include "xml2json.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "json11.hxx"
#include "mongoClient.h"
#include "frozen.h"

using namespace std;

namespace durian {

  namespace XmlElement {
    class Root;
    class Body;
    class Message;
  }

  template<typename xmlelement>
  class base {
  public:
    base(){}
    ~base(){}
    std::string tpl;

    template<typename T>
    string compile(const shared_ptr<T> o){

      // read entire template file 
      std::ifstream file(tpl.c_str());
      std::string tplx((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

      if (tplx.size() == 0){
        return "";
      }

      // compile template with context to create a soap message
      Plustache::template_t t;
      std::string result = t.render(tplx, *o);

      return result;
    }
  };

  // children must implement
  template<typename xmlelement>
  class generator_impl : public base<xmlelement>{};

  /*
    Root
  */
  template<>
  class generator_impl<XmlElement::Root> : public base<XmlElement::Root>{
  public:
    generator_impl(){ tpl = "tpl/root"; }

  };

  /*
    Body
  */
  template<>
  class generator_impl<XmlElement::Body> : public base<XmlElement::Body>{
  public:
    generator_impl(){ tpl = "tpl/body"; }

  };
  
  template<typename xmlelement>
  class generator{
  public:
    generator(){}
    ~generator(){}
  };
  
  template<>
  class generator<XmlElement::Root>{
  public:
    
    std::shared_ptr<std::string> compile(shared_ptr<PlustacheTypes::ObjectType> ctx, std::string& tplPath) {

      generator_impl<XmlElement::Root> elemGen;
      elemGen.tpl = tplPath;
      auto genTpl = elemGen.compile(ctx);
      auto r = make_shared<string>(genTpl);

      return r;
    }

  };

  template<>
  class generator<XmlElement::Body>{
  public:

    std::shared_ptr<std::string> compile(shared_ptr<Plustache::Context> ctx, std::string& tplPath) {

      generator_impl<XmlElement::Body> elemGen;
      elemGen.tpl = tplPath;
      auto genTpl = elemGen.compile(ctx);
      auto r = make_shared<string>(genTpl);

      return r;
    }    

  };

  template<>
  class generator<XmlElement::Message> {

  public:
    shared_ptr<string> render(shared_ptr<Plustache::Context> ctx, const string _path, const string _rootPath) {
      string tplPath(_path);
      durian::generator<durian::XmlElement::Body> gen;
      auto body = gen.compile(ctx, tplPath);

      // create envelope wrapper
      auto rootCtx = make_shared<PlustacheTypes::ObjectType>();
      (*rootCtx)["body"] = *body;
      
      string rootPath(_rootPath);
      durian::generator<durian::XmlElement::Root> rootGen;
      auto message = rootGen.compile(rootCtx, rootPath);

      return message;
    }
  };

  /*
    client implenmentation
    LOB client modules should derive from this base class
  */
  template<typename socketType>
  class client {

  public:

    client<socketType>(const char* _host,
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
    ~client<socketType>(){}
    client() = default;

    std::function<string(boost::future<std::string>)> loginResponse(shared_ptr<Plustache::Context> ctx) {
      return [&](boost::future<std::string> f) ->string{
        auto res = f.get();
        auto tok = getToken(res);
        ctx->add("token", tok);
        return tok;
      };
    }

    // Logout
    std::function<string(boost::future<std::string>)> logout(shared_ptr<Plustache::Context> ctx, const string bodyTpl, const string rootTpl) {
      //auto logout = [&](boost::future<std::string> f){
      return [&,bodyTpl,rootTpl](boost::future<std::string> f)->string{
      // the session token is in the ctx, it will be sent to the server to end session
        auto logoutXml = this->generator.render(ctx, bodyTpl, rootTpl);
        auto future = apiClient->methodPromise["POST"];
        auto pms = future(servicePath, *logoutXml, customHeaders);
        return pms.get();
      };
    }

    // Logout response
    std::function<string(boost::future<std::string>)> logoutResponse() {
      return [&](boost::future<std::string> f)->string{
        //auto logoutResponse = [&](boost::future<std::string> f){
        auto resp = f.get();
        // dumpFile("logoutResponse", resp.c_str());
        return resp;
      };
    }

  protected:
    
    shared_ptr<Plustache::Context> ctx;
    durian::generator<XmlElement::Message> generator;
    unique_ptr<api::client<socketType>> apiClient;
    PlustacheTypes::ObjectType user, password, token, response;
    std::map<string, string> customHeaders;
    string host, servicePath, userstr, passwordstr;

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

  /*
  utils
  */
  static void dumpFile(const char* fname, const char* content) {
    ofstream f;
    f.open(fname);
    f << content;
    f.close();
  }

  static void dumpPrettyJson(const char* outfile, rapidjson::Document& d) {
    // stringify prettyprint
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    d.Accept(writer);
    dumpFile(outfile, sb.GetString());
  }

  static void createContextFromJson(const char* rawJson, std::list<const char*>& selectors, shared_ptr<Plustache::Context> ctx) {

    struct json_token *arr, *tok;
    arr = parse_json2(rawJson, strlen(rawJson));
    for (auto& e : selectors){
      tok = find_json_token(arr, e);
      ctx.get()->add(string(e), string(tok->ptr, tok->len));
    }
    free(arr);
  }

}

#endif