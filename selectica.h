#ifndef SELECTICA_H
#define SELECTICA_H

#include "durian.h"

using namespace std;
using namespace durian;

namespace selectica {
  
  class client {
    durian::generator<XmlElement::Message> generator;
  public:
    client(){}
    ~client(){}
    
    shared_ptr<string> login(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/Login", "tpl/selectica/root");
    }

    shared_ptr<string> logout(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/Logout", "tpl/selectica/root");
    }

    shared_ptr<string> getBOTypes(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/GetBOTypes", "tpl/selectica/root");
    }

    shared_ptr<string> getBOMetaData(shared_ptr<Plustache::Context> ctx) {
      return this->generator.render(ctx, "tpl/selectica/getBOMetaData", "tpl/selectica/root");
    }
    
  };

}

#endif