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


  };
}

#endif