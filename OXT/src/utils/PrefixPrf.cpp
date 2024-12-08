#include "utils/PrefixPrf.h"
#include "utils/GGMNode.h"
#include "utils/Tools.h"
#include <cmath>
#include <string>

long GenIndex(const std::string &bin) {
  int idx = 0;
  for (int i = 0; i < bin.size(); i++) {
    idx += (bin[i] - '0') * pow(2, i);
  }
  return idx;
}

GGMNode PrefixPrf::Cons(const std::string &prefix,
                        const std::string &masterKey) {
  // trans prefix into bits
  // std::string binStr = IntToBinStr(std::stoi(prefix));
  std::string bin = StrToBin(prefix);
  long index = GenIndex(bin);
  int level = bin.size();
  GGMNode consKey = GGMNode(index, level);

  // derive key for consNode
  std::string current_key = masterKey;
  std::string next_key;
  // does not need to derive
  for (int i = 0; i < bin.size(); i++) {
    int bit = bin[i] - '0';
    next_key = CryptoTools::Prf_key_256(std::to_string(bit), current_key);
    current_key = next_key;
    // std::cout << "Cons, current key: " << StrToHex(current_key) << std::endl;
  }
  consKey.key_ = current_key;
  return consKey;
}

std::string PrefixPrf::Eval(const GGMNode &consKey, const std::string &x) {
  // trans x into binary str
  std::string binStr = StrToBin(x);

  // get the value of leaf node
  std::string current_key = consKey.key_;
  std::string next_key;

  for (int i = 0; i < binStr.size(); i++) {
    int bit = binStr[i] - '0';
    next_key = CryptoTools::Prf_key_256(std::to_string(bit), current_key);
    current_key = next_key;
  }
  return current_key;
}

std::string PrefixPrf::DirectEval(const std::string &x,
                                  const std::string &masterKey) {
  // trans prefix into bits
  std::string binStr = StrToBin(x);
  // std::cout << "Deval, str: " << binStr << std::endl;
  //  derive key for consNode
  std::string current_key = masterKey;
  std::string next_key;
  // does not need to derive
  for (int i = 0; i < binStr.size(); i++) {
    int bit = binStr[i] - '0';
    next_key = CryptoTools::Prf_key_256(std::to_string(bit), current_key);
    current_key = next_key;
    // std::cout << "Deval, current key: " << stringToHex(current_key)
    //<< std::endl;
  }
  return current_key;
}

int PrefixPrf::get_level() const { return this->width_; }
