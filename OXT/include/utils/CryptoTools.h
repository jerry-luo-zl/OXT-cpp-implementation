#ifndef CRYPTO_H_
#define CRYPTO_H_
#include <cstddef>
#include <memory>
#include <openssl/evp.h>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>

#include <string>

#include <random>

namespace CryptoTools {

typedef unsigned char u_8;
using EVP_CIPHER_CTX_ptr =
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;
#define KEY_SIZE 16
#define BLOCK_SIZE 16
#define DIGEST_LEN 16
#define GGM_SIZE 110000
#define PRF_SIZE 32

// AES encyption in CTR mode
std::string AesEncrypt(const std::string &key, const std::string &iv,
                       const std::string &ptext);
// AES decryption in CTR mpde
std::string AesDecrypt(const std::string &key, const std::string &iv,
                       std::string &ctext);

// PRF implementation--HMAC
std::string Prf(const std::string &msg, const std::string &key);

// PRF with key size 256
std::string Prf_key_256(const std::string &msg, const std::string &key);

// check if ID is valid
bool IsValidId(const std::string &s);

} // namespace CryptoTools
#endif