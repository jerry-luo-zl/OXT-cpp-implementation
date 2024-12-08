#include <cstring>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <utils/CryptoTools.h>

TEST(CryptoToolsTest, AESCTRTest) {
  std::string key = "1234567899123456";
  std::string iv = "1234567899123456";

  std::string ptext = "12345";
  std::string ptext1;
  std::string ctext;
  ctext = CryptoTools::AesEncrypt(key, iv, ptext);
  std::cout << "Plain: " << ptext << std::endl;
  std::cout << "Cipher: " << ctext << std::endl;
  ptext1 = CryptoTools::AesDecrypt(key, iv, ctext);
  std::cout << "Recover: " << ptext1 << std::endl;
  EXPECT_EQ(ptext, ptext1);
}

TEST(CryptoToolsTest, PRFTest) {

  std::string key = "1234567891234567";
  std::string ptext = "12345123";
  std::string ptext2 = "aasda";

  std::string c1 = CryptoTools::Prf(ptext, key);
  std::string c2 = CryptoTools::Prf(ptext, key);
  std::string c3 = CryptoTools::Prf(ptext2, key);

  auto transToHex = [](std::string c) -> std::string {
    std::stringstream ss;
    ss << std::hex;
    for (char ch : c) {
      ss << static_cast<int>(static_cast<unsigned char>(ch));
    }
    return ss.str();
  };
  std::cout << ptext << " : " << transToHex(c1) << std::endl;
  std::cout << ptext << " : " << transToHex(c2) << std::endl;
  std::cout << ptext2 << " : " << transToHex(c3) << std::endl;
}