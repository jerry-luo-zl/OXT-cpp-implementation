#ifndef BLOOM_FILTER_BLOOM_FILTER_H_
#define BLOOM_FILTER_BLOOM_FILTER_H_

#include "utils/CompressBF.h"
#include "utils/SpookyV2.h"
#include <algorithm>
#include <assert.h>
#include <climits>
#include <cstddef>
#include <sstream>
#include <vector>

namespace bloomfilter {
// status returned by a Bloom filter operation
enum Status {
  Ok = 0,
  NotFound = 1,
  NotEnoughSpace = 2,
  NotSupported = 3,
};

inline uint32_t reduce(uint32_t hash, uint32_t n) {
  // http://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
  return (uint32_t)(((uint64_t)hash * n) >> 32);
}

/**
 * Given a value "word", produces an integer in [0,p) without division.
 * The function is as fair as possible in the sense that if you iterate
 * through all possible values of "word", then you will generate all
 * possible outputs as uniformly as possible.
 */
static inline uint32_t fastrange32(uint32_t word, uint32_t p) {
  // http://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
  return (uint32_t)(((uint64_t)word * (uint64_t)p) >> 32);
}

#if defined(_MSC_VER) && defined(_WIN64)
#include <intrin.h> // should be part of all recent Visual Studio
#pragma intrinsic(_umul128)
#endif // defined(_MSC_VER) && defined (_WIN64)

/**
 * Given a value "word", produces an integer in [0,p) without division.
 * The function is as fair as possible in the sense that if you iterate
 * through all possible values of "word", then you will generate all
 * possible outputs as uniformly as possible.
 */
static inline uint64_t fastrange64(uint64_t word, uint64_t p) {
  // http://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
#ifdef __SIZEOF_INT128__ // then we know we have a 128-bit int
  return (uint64_t)(((__uint128_t)word * (__uint128_t)p) >> 64);
#elif defined(_MSC_VER) && defined(_WIN64)
  // supported in Visual Studio 2005 and better
  uint64_t highProduct;
  _umul128(word, p, &highProduct); // ignore output
  return highProduct;
  unsigned __int64 _umul128(unsigned __int64 Multiplier,
                            unsigned __int64 Multiplicand,
                            unsigned __int64 *HighProduct);
#else
  return word % p; // fallback
#endif // __SIZEOF_INT128__
}

#ifndef UINT32_MAX
#define UINT32_MAX (0xffffffff)
#endif // UINT32_MAX

/**
 * Given a value "word", produces an integer in [0,p) without division.
 * The function is as fair as possible in the sense that if you iterate
 * through all possible values of "word", then you will generate all
 * possible outputs as uniformly as possible.
 */
static inline size_t fastrangesize(uint64_t word, size_t p) {
#if (SIZE_MAX == UINT32_MAX)
  return (size_t)fastrange32(word, p);
#else  // assume 64-bit
  return (size_t)fastrange64(word, p);
#endif // SIZE_MAX == UINT32_MAX
}

static inline size_t getBestK(size_t bitsPerItem) {
  return std::max(1, (int)round((double)bitsPerItem * log(2)));
}

inline uint64_t getBit(uint32_t index) { return 1L << (index & 63); }

class BloomFilter {
public:
  uint64_t *data;
  size_t size;
  size_t arrayLength;
  size_t bitCount;
  int kk;
  int k;
  size_t bits_per_item;
  SpookyHash hasher;
  const bool branchless = false;

  double BitsPerItem() const { return k; }

  explicit BloomFilter(const size_t n, const size_t bpi) : hasher() {
    this->bits_per_item = bpi;
    this->size = n;
    this->k = (int)((double)bits_per_item * 0.693147180559945 + 0.5);
    this->kk = getBestK(bits_per_item);
    this->bitCount = n * bits_per_item;
    this->arrayLength = (bitCount + 63) / 64;
    data = new uint64_t[arrayLength];
    std::fill_n(data, arrayLength, 0);
  }
  explicit BloomFilter(std::vector<uint64> arr) {}

  explicit BloomFilter(CompressedBF cbf) {
    this->bits_per_item = cbf.bits_per_item;
    this->size = cbf.size;
    this->k = (int)((double)bits_per_item * 0.693147180559945 + 0.5);
    this->kk = getBestK(bits_per_item);
    this->bitCount = size * bits_per_item;
    this->arrayLength = (bitCount + 63) / 64;
    this->data = new uint64_t[arrayLength];
    for (int i = 0; i < arrayLength; i++) {
      this->data[i] = static_cast<uint64>(cbf.data[i]);
    }
    this->hasher = SpookyHash(static_cast<uint64_t>(cbf.seed));
  }

  CompressedBF Compress() {
    CompressedBF cbf;
    cbf.bits_per_item = this->bits_per_item;
    cbf.size = this->size;
    cbf.seed = static_cast<int64_t>(this->hasher.getSeed());
    for (int i = 0; i < arrayLength; i++) {
      cbf.data.emplace_back(static_cast<int64_t>(this->data[i]));
    }
    return cbf;
  }

  ~BloomFilter() { delete[] data; }

  // Add an item to the filter.
  Status Add(const std::string &item);

  // Add multiple items to the filter.
  Status AddAll(const std::vector<std::string> &keys);
  // Report if the item is inserted, with false positive rate.
  Status Contain(const std::string &item);

  /* methods for providing stats  */
  // summary infomation
  std::string Info() const;

  // number of current inserted items;
  size_t Size() const { return size; }

  // size of the filter in bytes.
  size_t SizeInBytes() const { return arrayLength * 8; }
};

const int blockShift = 15;
const int blockLen = 1 << blockShift;

void applyBlock(uint32_t *tmp, int block, int len, uint64_t *data);

char bittest64(const uint64_t *t, uint64_t bit);

} // namespace bloomfilter
#endif // BLOOM_FILTER_BLOOM_FILTER_H_
