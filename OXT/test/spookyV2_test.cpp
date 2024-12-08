#include <cstring>
#include <gtest/gtest.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <utils/SpookyV2.h>

TEST(SpookyV2Test, SimpleTest) {
  std::string p1 = "123456";
  std::string p2 = "123456";
  std::string p3 = "abcdef";
  std::string p4 = "123457";
  SpookyHash hasher = SpookyHash();
  auto c1 = hasher.Hash64(&p1[0], p1.size());
  auto c2 = hasher.Hash64(&p2[0], p2.size());
  auto c3 = hasher.Hash64(&p3[0], p3.size());
  auto c4 = hasher.Hash64(&p4[0], p4.size());

  std::cout << "c1 : " << c1 << std::endl;
  std::cout << "c2 : " << c2 << std::endl;
  std::cout << "c3 : " << c3 << std::endl;
  std::cout << "c4 : " << c4 << std::endl;
}