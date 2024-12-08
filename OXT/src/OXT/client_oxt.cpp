#include "utils/Config.h"
#include "utils/Tools.h"
#include <OXT/SSEClient.h>
#include <filesystem>
#include <iostream>
#include <ostream>

int main() {

  std::string sterm = "page_len,24";
  std::string xterm = "page_namespace,14";
  std::cout << "=============Test for OXT=============" << std::endl;
  std::cout << "dbName : " << dbName << std::endl;
  std::cout << "tabName : " << tabName << std::endl;
  std::cout << "EDBRootDir : " << EDBRootDir << std::endl;
  std::cout << "sterm : " << sterm << std::endl;
  std::cout << "xterm : " << xterm << std::endl;

  OXT::SSEClient client = OXT::SSEClient();

  std::vector<std::string> res;
  if (!std::filesystem::exists(EDBRootDir + "OXT_TSet.txt") ||
      !std::filesystem::exists(EDBRootDir + "OXT_XSet.txt") ||
      !std::filesystem::exists(EDBRootDir + "OXT_counter.txt") ||
      !std::filesystem::exists(EDBRootDir + "OXT_g.txt")) {
    std::cout << "Client: building EDB!" << std::endl;
    std::uint64_t t1 = NowTime();
    client.EDBSetup();
    // client.Search(res, sterm, xterm);
    std::uint64_t t2 = NowTime();
    std::cout << "Client: EDBSetup time: " << t2 - t1 << std::endl;
  } else {
    client.ReadCounter();
    std::uint64_t t1 = NowTime();
    std::cout << "Client: issue search!" << std::endl;
    // client.EDBSetup();
    client.Search(res, sterm, xterm);
    std::cout << "Client: Result size: " << res.size() << std::endl;
    std::uint64_t t2 = NowTime();
    std::cout << "Client: Search lantency: " << t2 - t1 << std::endl;

    /*
    for (auto t : res) {
      std::cout << t << std::endl;
    }
    */
    std::cout << "=============End test for OXT==============" << std::endl;
    std::cout << std::endl;
  }
}