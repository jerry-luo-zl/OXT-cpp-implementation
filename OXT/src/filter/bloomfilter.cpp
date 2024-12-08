#include "filter/bloom.h"

#include "utils/SpookyV2.h"
#include <algorithm>
#include <assert.h>
#include <climits>
#include <sstream>
#include <string>
#include <vector>

namespace bloomfilter {

Status BloomFilter::Add(const std::string &key) {
  uint64_t hash = hasher.Hash64(&key[0], key.size());
  uint64_t a = (hash >> 32) | (hash << 32);
  uint64_t b = hash;
  for (int i = 0; i < k; i++) {
    // int index = reduce(a, this->bitCount);
    // data[index >> 6] |= getBit(index);
    // reworked to avoid overflows
    // use the fact that reduce is not very sensitive to lower bits of a
    data[fastrangesize(a, this->arrayLength)] |= getBit(a);
    a += b;
  }
  return Ok;
}

void applyBlock(uint32_t *tmp, int block, int len, uint64_t *data) {
  for (int i = 0; i < len; i++) {
    uint32_t index = tmp[(block << blockShift) + i];
    data[index >> 6] |= getBit(index);
  }
}

Status BloomFilter::AddAll(const std::vector<std::string> &keys) {
  // we have that AddAll assumes that arrayLength << 6 is a
  // 32-bit integer
  if (arrayLength > 0x3ffffff) {
    for (size_t i = 0; i < keys.size(); i++) {
      Add(keys[i]);
    }
    return Ok;
  }
  int blocks = 1 + arrayLength / blockLen;
  uint32_t *tmp = new uint32_t[blocks * blockLen];
  int *tmpLen = new int[blocks]();
  for (size_t i = 0; i < keys.size(); i++) {
    std::string key = keys[i];
    uint64_t hash = hasher.Hash64(&key[0], key.size());
    uint64_t a = (hash >> 32) | (hash << 32);
    uint64_t b = hash;
    for (int j = 0; j < k; j++) {
      int index = fastrangesize(a, this->arrayLength);
      int block = index >> blockShift;
      int len = tmpLen[block];
      tmp[(block << blockShift) + len] = (index << 6) + (a & 63);
      tmpLen[block] = len + 1;
      if (len + 1 == blockLen) {
        applyBlock(tmp, block, len + 1, data);
        tmpLen[block] = 0;
      }
      a += b;
    }
  }
  for (int block = 0; block < blocks; block++) {
    applyBlock(tmp, block, tmpLen[block], data);
  }
  delete[] tmp;
  delete[] tmpLen;
  return Ok;
}

char bittest64(const uint64_t *t, uint64_t bit) {
  return (*t & (1L << (bit & 63))) != 0;
}

Status BloomFilter::Contain(const std::string &key) {
  uint64_t hash = hasher.Hash64(&key[0], key.size());
  uint64_t a = (hash >> 32) | (hash << 32);
  uint64_t b = hash;
  if (branchless && k >= 3) {
    int b0 = data[fastrangesize(a, this->arrayLength)] >> (a & 63);
    a += b;
    int b1 = data[fastrangesize(a, this->arrayLength)] >> (a & 63);
    a += b;
    int b2 = data[fastrangesize(a, this->arrayLength)] >> (a & 63);
    if ((b0 & b1 & b2 & 1) == 0) {
      return NotFound;
    }
    for (int i = 3; i < k; i++) {
      a += b;
      if (((data[fastrangesize(a, this->arrayLength)] >> (a & 63)) & 1) == 0) {
        return NotFound;
      }
    }
    return Ok;
  }
  for (int i = 0; i < k; i++) {
    if ((data[fastrangesize(a, this->arrayLength)] & getBit(a)) == 0) {
      return NotFound;
    }
    a += b;
  }
  return Ok;
}

std::string BloomFilter::Info() const {
  std::stringstream ss;
  ss << "BloomFilter Status:\n"
     << "\t\tKeys stored: " << Size() << "\n";
  if (Size() > 0) {
    ss << "\t\tk:   " << BitsPerItem() << "\n";
  } else {
    ss << "\t\tk:   N/A\n";
  }
  return ss.str();
}

} // namespace bloomfilter