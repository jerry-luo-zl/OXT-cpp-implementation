#include "utils/GGMNode.h"
#include "utils/PrefixPrf.h"
#include "utils/SpookyV2.h"
#include "utils/Tools.h"
#include <cstring>
#include <filter/xorfilter_modi.h>
#include <gtest/gtest.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

TEST(PrefixPrf, SimpleTest) {
  std::string key(PRF_SIZE, '1');
  int width = 32;
  PrefixPrf pprf = PrefixPrf(width * 2);
  std::string prefix = "12305:";
  GGMNode consKey = pprf.Cons(prefix, key);
  std::string x = "45";
  std::string Res1 = pprf.Eval(consKey, x);
  std::string directX = "12305:45";
  std::string Res2 = pprf.DirectEval(directX, key);
  std::cout << "Res1: " << StrToHex(Res1) << std::endl;
  std::cout << "Res2: " << StrToHex(Res2) << std::endl;
  EXPECT_EQ(Res1, Res2);
}
