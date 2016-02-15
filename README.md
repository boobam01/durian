# durian

The fourth of many fruits.  Stinky. :stuck_out_tongue_closed_eyes:

####Use durian with promises
This is an example of invoking a SOAP action.
```cpp
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
```