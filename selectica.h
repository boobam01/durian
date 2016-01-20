#ifndef SELECTICA_H
#define SELECTICA_H

#include "durian.h"

using namespace std;
using namespace durian;

namespace selectica {
  
  class client {

  public:
    client(){}
    ~client(){}
    
    shared_ptr<string> render(shared_ptr<Plustache::Context> ctx, char* _path) {

      string tplPath(_path);
      durian::generator<durian::XmlElement::Body> gen;
      auto res = gen.compile(ctx, tplPath);
      return res;

    }

    shared_ptr<string> login(shared_ptr<Plustache::Context> ctx) {
      return this->render(ctx, "tpl/selectica/Login");      
    }

    shared_ptr<string> logout(shared_ptr<Plustache::Context> ctx) {
      return this->render(ctx, "tpl/selectica/Logout");
    }

    shared_ptr<string> getBOTypes(shared_ptr<Plustache::Context> ctx) {
      return this->render(ctx, "tpl/selectica/GetBOTypes");
    }

    shared_ptr<string> getBOMetaData(shared_ptr<Plustache::Context> ctx) {
      return this->render(ctx, "tpl/selectica/getBOMetaData");
    }
    
  };

}

#endif