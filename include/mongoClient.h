#pragma once

#ifndef MONGO_CLIENT_H
#define	MONGO_CLIENT_H

#include <iostream>
#include <mongo/client/dbclient.h>
#include "spdlog/spdlog.h"

using namespace std;

namespace mongoclient {

  class client {
    string host, port, database;
    mongo::DBClientConnection conn;
  public:
    client(const string mHost, const string mPort, const string mDatabase) : host(mHost), port(mPort), database(mDatabase) {}
    ~client(){}
    int connect() {
      std::string uri = host + ":" + port;
      try {
        conn.connect(uri);
      }
      catch (const mongo::DBException &e){
        spdlog::get("logger")->error() << "failed to initialize the client driver";
        return EXIT_FAILURE;
      }
      return 0;
    }

    std::auto_ptr<mongo::DBClientCursor> find(const string collection, mongo::Query cri, int limit, int skip, mongo::BSONObj* selector){
      auto cur = conn.query(database + "." + collection, cri, limit, skip, selector, 0, 0);
      return cur;
    }

    std::shared_ptr<mongo::BSONObj> findOne(const string collection, mongo::Query cri, mongo::BSONObj* selector){
      auto resp = conn.findOne(database + "." + collection, cri, selector, 0);
      auto o = make_shared<mongo::BSONObj>(resp);
      return o;
    }

    std::function<boost::future<std::string>(const string, mongo::Query, mongo::BSONObj*)> findOnePromise() {
      return [this](const string collection, mongo::Query cri, mongo::BSONObj* selector)->boost::future < string > {
        return boost::async([=]()->string {
          auto resp = conn.findOne(database + "." + collection, cri, selector, 0);
          auto o = resp.jsonString(mongo::Strict, 0, false);
          return o;
        });
      };
    }

    /**
    MongoDB bulk operation
    **/
    string bulkToMongo(const string database, const string collection, vector<string> v) {
      string resp = "";
      string ns = database + "." + collection;
      
      auto bulk = this->conn.initializeUnorderedBulkOp(ns);
      for (auto& w : v) {
        auto o = mongo::fromjson(w);
        if (o.hasField("_id")) {
          auto v = o["_id"];
          auto nb = v.wrap();
          bulk.find(nb).upsert().replaceOne(o);
        }
      }
      mongo::WriteConcern wc;
      mongo::WriteResult wr;
      bulk.execute(&wc, &wr);

      if (wr.hasWriteErrors()){
        for (auto& bo : wr.writeErrors()){
          resp += "Bulk upsert for ns => " + ns + " writeError => " + bo.jsonString() + "\n";
        }
      }
      auto num = wr.nUpserted();
      auto num1 = wr.nModified();
      stringstream ss;
      ss << "Bulk upsert for ns => " << ns << " nUpserted => " << num << " nModified => " << num1 << " total => " << (num + num1);
      resp += ss.str() + "\n";
      return resp;
    }

  };
}

#endif