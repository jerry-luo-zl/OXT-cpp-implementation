#ifndef XOR_FILTER_MODI_H_
#define XOR_FILTER_MODI_H_

#include "utils/SpookyV2.h"
#include <algorithm>
#include <assert.h>
#include <climits>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace xorfilter {
// status returned by a xor filter operation
enum Status {
  Ok = 0,
  NotFound = 1,
  NotEnoughSpace = 2,
  NotSupported = 3,
};

__attribute__((always_inline)) inline uint64_t rotl64(uint64_t n,
                                                      unsigned int c) {
  // assumes width is a power of 2
  const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);
  // assert ( (c<=mask) &&"rotate by type width or more");
  c &= mask;
  return (n << c) | (n >> ((-c) & mask));
}

__attribute__((always_inline)) inline uint32_t reduce(uint32_t hash,
                                                      uint32_t n) {
  // http://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
  return (uint32_t)(((uint64_t)hash * n) >> 32);
}

__attribute__((always_inline)) inline size_t
getHashFromHash(uint64_t hash, int index, int blockLength) {
  uint32_t r = rotl64(hash, index * 21);
  return (size_t)reduce(r, blockLength) + index * blockLength;
}

class XorFilter {
public:
  size_t size;
  size_t arrayLength;
  size_t blockLength;
  size_t fingerprintLen;
  std::string *fingerprints;

  SpookyHash hasher;
  size_t hashIndex{0};

  inline std::string FingerprintXor(const std::string &s1,
                                    const std::string &s2) {
    int llen = s1.size() > s2.size() ? s2.size() : s1.size();
    int glen = s1.size() > s2.size() ? s1.size() : s2.size();
    std::string res = s1.size() > s2.size() ? s1 : s2;
    for (int i = 0; i < llen; i++) {
      res[i] = s1[i] ^ s2[i];
    }
    return res;
  }

  inline std::string GenRandomString(int length) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, sizeof(charset) - 2);

    std::string str;
    str.reserve(length);
    for (int i = 0; i < length; ++i) {
      str += charset[distribution(rng)];
    }

    return str;
  }

  explicit XorFilter() {}
  explicit XorFilter(const size_t size) {
    hasher = SpookyHash();
    this->size = size;
    this->arrayLength = 32 + 1.23 * size;
    this->blockLength = arrayLength / 3;
    // this->fingerprintLen = fingerprintLen;
    fingerprints = new std::string[this->arrayLength];
  }

  ~XorFilter() { delete[] fingerprints; }

  explicit XorFilter(const std::string &filename);

  void Serialize(const std::string &filename);

  Status AddAll(std::map<std::string, std::string> &kvPairs);

  // Report if the item is inserted, with false positive rate.
  Status Contain(const std::string &item,
                 std::map<std::string, std::string> &kvPairs);

  std::string Retrieval(const std::string &key);

  /* methods for providing stats  */
  // summary infomation
  std::string Info() const;

  // number of current inserted items;
  size_t Size() const { return size; }

  // size of the filter in bytes.
  size_t SizeInBytes() const { return arrayLength * sizeof(std::string); }
};

struct t2val {
  uint64_t t2;
  uint64_t t2count;
};

typedef struct t2val t2val_t;

const int blockShift = 18;

void applyBlock(uint64_t *tmp, int b, int len, t2val_t *t2vals);

int applyBlock2(uint64_t *tmp, int b, int len, t2val_t *t2vals, int *alone,
                int alonePos);

} // namespace xorfilter
#endif // XOR_FILTER_XOR_FILTER_H_
