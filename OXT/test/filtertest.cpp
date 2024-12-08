#include "filter/bloom.h"
#include "utils/CompressBF.h"
#include "utils/PrefixPrf.h"
#include "utils/SpookyV2.h"
#include "utils/Tools.h"
#include <cstring>
#include <filter/xorfilter_modi.h>
#include <gtest/gtest.h>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

TEST(FilterTest, XorFilterTest) {
  using namespace xorfilter;
  std::map<std::string, std::string> kvPairs;
  int size = 20000;
  XorFilter xorFilter = XorFilter(size);
  // add items
  int c1 = 0;
  int c2 = 10000;
  for (int i = 0; i < size; i++) {
    kvPairs[std::to_string(c1)] = std::to_string(c2);
    c1++;
    c2++;
  }
  xorFilter.AddAll(kvPairs);

  // contains
  for (int i = 0; i < size; i++) {
    // std::cout << "Exp: " << kvPairs[to_string(i)] << endl;
    // std::cout << xorFilter.Contain(std::to_string(i), kvPairs) << endl;
    // std::cout << xorFilter.Retrieval(std::to_string(i)) << std::endl;
    // !!!!!!!!!!!!!可能tail会有多余的\0
    EXPECT_EQ(xorFilter.Contain(std::to_string(i), kvPairs), 0);
    EXPECT_EQ(StrStrip(xorFilter.Retrieval(std::to_string(i))),
              kvPairs[to_string(i)]);
  }

  std::cout << "Origin:" << std::endl;
  for (int i = 0; i < xorFilter.arrayLength; i++) {
    std::cout << StrToHex(xorFilter.fingerprints[i]) << std::endl;
  }

  // serilization
  xorFilter.Serialize("/Volumes/solid/code/project/test/filter.txt");

  XorFilter newFilter =
      XorFilter("/Volumes/solid/code/project/test/filter.txt");

  std::cout << "After:" << std::endl;
  for (int i = 0; i < newFilter.arrayLength; i++) {
    std::cout << StrToHex(newFilter.fingerprints[i]) << std::endl;
  }

  // contains
  for (int i = 0; i < size; i++) {
    // std::cout << "Exp: " << kvPairs[to_string(i)] << endl;
    // std::cout << xorFilter.Contain(std::to_string(i), kvPairs) << endl;
    // std::cout << xorFilter.Retrieval(std::to_string(i)) << std::endl;
    // !!!!!!!!!!!!!可能tail会有多余的\0
    EXPECT_EQ(newFilter.Contain(std::to_string(i), kvPairs), 0);
    EXPECT_EQ(StrStrip(newFilter.Retrieval(std::to_string(i))),
              kvPairs[to_string(i)]);
  }
}

TEST(DISABLED_FilterTest, BloomFilterTest) {
  using namespace bloomfilter;
  int size = 100;
  int bits_per_item = 12;
  BloomFilter bf = BloomFilter(size, bits_per_item);
  vector<std::string> keys;
  for (int i = 0; i < size; i++) {
    keys.emplace_back(std::to_string(i));
  }
  bf.AddAll(keys);
  int cnt = 0;
  for (int i = 0; i < size; i++) {
    if (bf.Contain(std::to_string(i)) == 1) {
      std::cout << "False negtive: " << i << std::endl;
    }
  }

  for (int i = size + 1; i < 2 * size; i++) {
    if (bf.Contain(std::to_string(i)) == 0) {
      std::cout << "False positive: " << i << std::endl;
      break;
    }
  }
  CompressedBF cbf = bf.Compress();
  BloomFilter bf1 = BloomFilter(cbf);
  for (int i = 0; i < size; i++) {
    if (bf1.Contain(std::to_string(i)) == 1) {
      std::cout << "Compress False negtive: " << i << std::endl;
      break;
    }
  }

  for (int i = size + 1; i < 2 * size; i++) {
    if (bf1.Contain(std::to_string(i)) == 0) {
      std::cout << "Compress  False positive: " << i << std::endl;
      break;
    }
  }
}