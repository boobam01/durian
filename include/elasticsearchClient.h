#ifndef ELASTICSEARCH_CLIENT_H
#define	ELASTICSEARCH_CLIENT_H

#include "api_client.h"
#include "spdlog/spdlog.h"
#include "json11.hxx"

using namespace json11;

namespace elasticsearch {
  template<typename socketType>
  class client {
  public:
    friend unique_ptr<api::client<socketType>> apiClient;
    client(string _host) : public api::client<T>(_host) {
      apiClient = make_unique<api::client<SimpleWeb::HTTP>>(host);
    }
    ~client(){}
    client() = default;
    /**
    store elasticsearch hits
    **/
    struct hit{
      hit(string index, string type, string id) : _index(index), _type(type), _id(id){}
      string _index;
      string _type;
      string _id;
      string _source;
    };
    
    virtual void sync(){};

    void saveHit(Json::object& j){
      //save this internally
      elasticsearch::client::hit h(j["_index"].string_value(), j["_type"].string_value(), j["_id"].string_value());
      //stringify the _source
      Json::object o = j["_source"];
      h._source = o.dump();
      this->hits.push_back(h);
    }

    virtual int getWork(){ return 0; };

    virtual int bulkToElastic(shared_ptr<vector<shared_ptr<string>>>& sv) {
      stringstream ss;
      auto v = *sv;

      if (v.size() == 0){
        return 1;
      }

      for (auto& s : v){
        ss << *s;
      }

      //start timer
      auto t1 = std::chrono::high_resolution_clock::now();

      

      HttpClient bulkClient(this->esconf["host"] + ":" + this->esconf["port"]);

      auto r = bulkClient.request("POST", "/_bulk", ss);
      ostringstream o;
      o << r->content.rdbuf();
      spdlog::get("logger")->info() << o.str();

      //string message = "Bulk to ES completed.  Elapsed => ";
      //timer(message, t1);
      ss.str("");
      ss << "Bulk to ES completed.  Total => " << v.size() << " Elapsed =>";
      timer(ss.str(), t1);

      return 0;
    }

    virtual void cleanupAndLog(){
      if (this->hits.size() == 0){
        //spdlog::get("logger")->info() << "cleanupAndLog() => Nothing to process";
        return;
      }

      auto b = shared_ptr<vector<shared_ptr<string>>>(new vector<shared_ptr<string>>);

      for (auto& input : this->hits){
        Json::Value meta;
        Json::Value body;
        stringstream ss;

        meta["update"]["_index"] = input._index;
        meta["update"]["_type"] = input._type;
        meta["update"]["_id"] = input._id;
        meta["update"]["_retry_on_conflict"] = 0;

        body["doc"]["processed"] = 1;

        spdlog::get("logger")->info() << "_index => " << input._index << " _type => " << input._type << " _id => " << input._id << " _source => " << input._source;

        //.write will tack on a \n after completion
        Json::FastWriter writer;
        auto compactMeta = writer.write(meta);
        auto compactBody = writer.write(body);

        ss << compactMeta << compactBody;

        auto sv = shared_ptr<string>(new string(ss.str()));
        b->push_back(sv);
      }

      this->bulkToElastic(b);
    }
  private:
    void timer(string& message, std::chrono::time_point<std::chrono::system_clock>& t1){
      stringstream ss;
      auto t2 = std::chrono::high_resolution_clock::now();
      ss.str("");
      ss << message << " "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        << " milliseconds\n";

      spdlog::get("logger")->info(ss.str());
      //force a flush
      spdlog::get("logger")->flush();
    }
  };
}

#endif