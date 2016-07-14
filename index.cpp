#include "durian.h"

int main(int argc, char *argv[]) {
  {
    // Test connection refused
    typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
    try {
      // Expect connection refused
      HttpClient client("localhost:1234");
      auto r1 = client.request("GET", "/");
      cout << r1->content.rdbuf() << endl;
    } catch (const std::exception& e) {
      cout << e.what() << endl;
    }
  }

  {
    // Test timeout
    typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
    try {
      HttpClient client("clcwcdcdvm003", 3000, 3000);
      // Expect timeout
      // message => "read_until: A connection attempt failed because the connected party did not properly respond after a period of time, 
      // or established connection failed because connected host has failed to respond"
      auto r1 = client.request("GET", "/ClickByPass/EntityManager/EntityServices.asmx");
      cout << r1->content.rdbuf() << endl;
    }
    catch (const std::exception& e) {
      cout << e.what() << endl;
    }
  }

  {
    class testClient : public durian::client<SimpleWeb::HTTP> {
    public:
    public:
      testClient(const char* _host,
        const char* _servicePath,
        const char* _user,
        const char* _password,
        std::map<string, string>& _customHeaders,
        unsigned int s_timeout = 30000,
        unsigned int r_timeout = 30000
        ) : durian::client<SimpleWeb::HTTP>(_host, _servicePath, _user, _password, _customHeaders, s_timeout, r_timeout) {}

      void get(string& params, string& data) {
        std::map<string, string> header{};
        auto future = this->apiClient->methodPromise["GET"];
        auto promises = future(params, data, header);
        auto resp = promises.get();        
      }
    };

    std::map < string, string > header{};

    // set client timeout to be 500 ms
    testClient t("localhost:1234", "/", "", "", header, 500, 500);
    t.get(string(""), string(""));
    
    // test connection refused
    testClient t1("rsnwcdcdvm001", "/ClickByPass/EntityManager/EntityServices.asmx", "", "", header, 500, 500);
    t1.get(string(""), string(""));

  }

  return 0;
}