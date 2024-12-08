#ifndef _OXT_SSESERVER_H_
#define _OXT_SSESERVER_H_
#include "SSEService.h"
#include "filter/bloom.h"
#include "utils/Config.h"
#include "utils/CryptoTools.h"
#include "utils/Tools.h"
#include <filesystem>
#include <fstream>
#include <gmp.h>
#include <iostream>
#include <mysql.h>
#include <ostream>
#include <pbc/pbc.h>
#include <pbc/pbc_field.h>
#include <string>
#include <vector>

using namespace bloomfilter;

namespace OXT {

typedef struct tSetTuple {
  std::string encID;
  std::string y;
  tSetTuple(std::string e_t, std::string y_t) : encID(e_t), y(y_t) {}
} tSetTuple;

class SSEServer : public SSEServiceIf {
public:
  SSEServer();

  void search(std::vector<std::string> &_return, const std::string &stag,
              const std::vector<std::string> &xtoken,
              const std::string &logPath);

private:
  std::unordered_map<std::string, std::vector<tSetTuple>> TSet_;
  BloomFilter *XSet_ = nullptr;
  pairing_t pairing_;
  element_t g_;

  int repeatCounter_;
  uint64_t serverTime_;
  int sampleCounter_;
};
} // namespace OXT
#endif