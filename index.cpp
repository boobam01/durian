#include <iostream>
#include <regex>
#include <boost/filesystem.hpp>
#include <fstream>
#include "selectica.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

auto setupLogging = []()->void {

  if (boost::filesystem::create_directory("./log")){
    boost::filesystem::path full_path(boost::filesystem::current_path());
    std::cout << "Successfully created directory"
      << full_path
      << "/log"
      << "\n";
  }

  size_t q_size = 1048576; //queue size must be power of 2
  spdlog::set_async_mode(q_size);

  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("log/durian", "txt", 0, 0));
  auto combined_logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
  combined_logger->set_pattern("[%Y-%d-%m %H:%M:%S:%e] [%l] [thread %t] %v");
  spdlog::register_logger(combined_logger);

};

namespace test {

  int spec1(){

    selectica::client client;

    auto ctx = make_shared<Plustache::Context>();
    
    // login
    auto tpl = client.login(ctx);

    // getBOTypes
    auto tpl1 = client.getBOTypes(ctx);

    // logout
    auto tpl2 = client.logout(ctx);

    return 0;
  }
}

int main(int argc, char *argv[]) {

  setupLogging();

  test::spec1();
}