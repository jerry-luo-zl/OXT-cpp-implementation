#include "utils/CryptoTools.h"
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <sstream>
#include <string>

namespace CryptoTools {

bool IsValidId(const std::string &s) {
  for (char c : s) {
    if (!std::isdigit(c)) {
      return false;
    }
  }
  return true;
}

std::string AesEncrypt(const std::string &key, const std::string &iv,
                       const std::string &ptext) {
  std::string ctext;
  EVP_CIPHER_CTX_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
  int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL,
                              (u_8 *)&key[0], (u_8 *)&iv[0]);
  if (rc != 1)
    throw std::runtime_error("EVP_EncryptInit_ex failed");

  // Cipher text expands upto BLOCK_SIZE
  ctext.resize(ptext.size() + BLOCK_SIZE);
  int out_len1 = (int)ctext.size();

  rc = EVP_EncryptUpdate(ctx.get(), (u_8 *)&ctext[0], &out_len1,
                         (const u_8 *)&ptext[0], (int)ptext.size());
  if (rc != 1)
    throw std::runtime_error("EVP_EncryptUpdate failed");

  int out_len2 = (int)ctext.size() - out_len1;
  rc = EVP_EncryptFinal_ex(ctx.get(), (u_8 *)&ctext[0] + out_len1, &out_len2);
  if (rc != 1)
    throw std::runtime_error("EVP_EncryptFinal_ex failed");
  // Set cipher text size now that we know it
  ctext.resize(out_len1 + out_len2);
  return ctext;
}

std::string AesDecrypt(const std::string &key, const std::string &iv,
                       std::string &ctext) {
  std::string ptext;
  EVP_CIPHER_CTX_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
  int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL,
                              (u_8 *)&key[0], (u_8 *)&iv[0]);
  // if (rc != 1)
  //  throw std::runtime_error("EVP_EncryptInit_ex failed");

  // Cipher text expands upto BLOCK_SIZE
  ptext.resize(ctext.size() + BLOCK_SIZE);
  int out_len1 = (int)ptext.size();

  rc = EVP_DecryptUpdate(ctx.get(), (u_8 *)&ptext[0], &out_len1,
                         (const u_8 *)&ctext[0], (int)ctext.size());
  // if (rc != 1)
  // throw std::runtime_error("EVP_EncryptUpdate failed");

  int out_len2 = (int)ptext.size() - out_len1;
  rc = EVP_DecryptFinal_ex(ctx.get(), (u_8 *)&ptext[0] + out_len1, &out_len2);
  // if (rc != 1)
  // throw std::runtime_error("EVP_EncryptFinal_ex failed");

  // Set cipher text size now that we know it
  ptext.resize(out_len1 + out_len2);
  return ptext;
}

std::string Prf(const std::string &msg, const std::string &key) {
  u_8 hash[PRF_SIZE];

  HMAC_CTX *hmac;
  hmac = HMAC_CTX_new();
  HMAC_Init_ex(hmac, (u_8 *)&key[0], key.size(), EVP_sha256(), NULL);
  HMAC_Update(hmac, (u_8 *)&msg[0], msg.length());
  unsigned int len = 32;
  HMAC_Final(hmac, hash, &len);
  HMAC_CTX_free(hmac);

  std::stringstream ss;
  ss << std::setfill('0');
  for (int i = 0; i < len; i++) {
    ss << hash[i];
  }
  return (ss.str());
}

std::string Prf_key_256(const std::string &msg, const std::string &key) {
  u_8 hash[PRF_SIZE];

  HMAC_CTX *hmac;
  hmac = HMAC_CTX_new();
  HMAC_Init_ex(hmac, (u_8 *)&key[0], key.size(), EVP_sha256(), NULL);
  HMAC_Update(hmac, (u_8 *)&msg[0], msg.length());
  unsigned int len = 32;
  HMAC_Final(hmac, hash, &len);
  HMAC_CTX_free(hmac);

  std::stringstream ss;
  ss << std::setfill('0');
  for (int i = 0; i < len; i++) {
    ss << hash[i];
  }
  return (ss.str());
}
} // namespace CryptoTools