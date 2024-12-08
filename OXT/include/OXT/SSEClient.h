#ifndef _OXT_SSECLIENT_H_
#define _OXT_SSECLIENT_H_

#include "OXT/SSEService.h"
#include "filter/bloom.h"
#include "utils/Config.h"
#include "utils/CryptoTools.h"
#include "utils/Tools.h"
#include <cstddef>
#include <filesystem>
#include <gmp.h>
#include <iostream>
#include <mysql.h>
#include <pbc/pbc.h>
#include <pbc/pbc_field.h>
#include <string>
#include <unordered_map>
#include <vector>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <vector>

using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace bloomfilter;

namespace OXT {

typedef struct tSetTuple {
  std::string encID;
  std::string y;
  tSetTuple(std::string e_t, std::string y_t) : encID(e_t), y(y_t) {}
} tSetTuple;

class SSEClient {
public:
  // parse plaintext DB
  // void ParseDB();
  ~SSEClient() {
    element_clear(g_);
    if (server_ != nullptr) {
      delete server_;
      server_ = nullptr;
    }
    if (XSet_ != nullptr) {
      delete XSet_;
      XSet_ = nullptr;
    }
  }

  SSEClient();
  void EDBSetup();
  void SerializeEDB();
  void ReadCounter();
  void Search(std::vector<std::string> &res, const std::string w1,
              const std::string w2, std::string logPath = "");
  void searchTest(std::vector<std::string> &_return, const std::string &stag,
                  const std::vector<std::string> &xtoken,
                  const std::string &logPath);

private:
  const std::string K0_ = "123456789ABCDEFG";
  const std::string KI_ = "123456789ABCD789";
  const std::string KZ_ = "908856789ABCD789";
  const std::string KX_ = "123856789ABCD789";
  const std::string Kenc_ = "1234567891111111";
  const std::string iv_ = "abcdefghijklmnop";
  MYSQL *conn_;
  std::string tabName_;
  std::unordered_map<std::string, std::vector<tSetTuple>> TSet_;
  BloomFilter *XSet_ = nullptr;

  SSEServiceClient *server_ = nullptr;
  std::shared_ptr<TTransport> transport_ = nullptr;
  pairing_t pairing_;
  element_t g_;

  std::string tSetPath_;
  std::string xSetPath_;
  std::string counterPath_;
  std::string gPath_;
  std::unordered_map<std::string, int> counter_;

  int columnCount_;
  std::vector<std::string> fieldNames;
};

} // namespace OXT
#endif