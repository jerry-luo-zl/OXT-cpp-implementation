#include "OXT/SSEServer.h"
#include "filter/bloom.h"
#include "utils/CompressBF.h"
#include "utils/CryptoTools.h"
#include "utils/Tools.h"
#include <cstdint>
#include <iostream>
#include <pbc/pbc_field.h>
#include <pbc/pbc_pairing.h>
#include <sstream>
#include <string>
#include <vector>
namespace OXT {

SSEServer::SSEServer() {
  std::string tSetPath = EDBRootDir + "OXT_TSet.txt";
  std::string xSetPath = EDBRootDir + "OXT_XSet.txt";
  std::string gPath = EDBRootDir + "OXT_g.txt";

  if (!std::filesystem::exists(tSetPath) ||
      !std::filesystem::exists(xSetPath) || !std::filesystem::exists(gPath)) {
    std::cerr << "No EDB, you should gen it first"
              << std::endl; // 输出错误信息到标准错误流
    std::exit(0);
  }

  // construct TSet from file
  std::ifstream in1(tSetPath);
  int tSetSize;
  in1.read(reinterpret_cast<char *>(&tSetSize), sizeof(tSetSize));

  for (int j = 0; j < tSetSize; j++) {
    std::string Fw;
    char buf[PRF_SIZE];
    in1.read(buf, PRF_SIZE);
    Fw = std::string(buf, PRF_SIZE);

    int cnt;
    in1.read(reinterpret_cast<char *>(&cnt), sizeof(cnt));
    for (int i = 0; i < cnt; i++) {
      int encIDSize;
      in1.read(reinterpret_cast<char *>(&encIDSize), sizeof(encIDSize));

      char buf1[encIDSize];
      std::string encID;
      in1.read(buf1, encIDSize);
      encID = std::string(buf1, encIDSize);

      int ySize;
      in1.read(reinterpret_cast<char *>(&ySize), sizeof(ySize));
      char buf2[ySize];
      std::string y;
      in1.read(buf2, ySize);
      y = std::string(buf2, ySize);

      TSet_[Fw].emplace_back(tSetTuple(encID, y));
    }
  }
  in1.close();

  // construct XSet from file
  std::ifstream in2(xSetPath);
  CompressedBF cbf;
  int64_t tmp;
  in2.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
  cbf.size = tmp;

  in2.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
  cbf.bits_per_item = tmp;

  in2.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
  cbf.seed = tmp;

  int size;
  in2.read(reinterpret_cast<char *>(&size), sizeof(size));
  for (int i = 0; i < size; i++) {
    int64_t tmp;
    in2.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
    cbf.data.emplace_back(tmp);
  }

  in2.close();

  XSet_ = new BloomFilter(cbf);

  // init paring
  char param[1024];
  FILE *file = fopen((EDBRootDir + "d159.param").c_str(), "r");
  size_t count = fread(param, 1, 1024, file);
  fclose(file);
  if (!count)
    pbc_die("input error");
  pairing_init_set_buf(pairing_, param, count);

  // construct g
  std::ifstream f(gPath);
  if (f.is_open()) {
    std::ostringstream oss;
    oss << f.rdbuf();
    std::string gStr = oss.str();
    f.close();
    element_init_G1(this->g_, pairing_);
    int ret = element_set_str(g_, gStr.c_str(), 10);
    if (ret == 0) {
      std::cout << "server parse g fail!" << std::endl; // exception
      exit(0);
    }
  }
  repeatCounter_ = 0;
  serverTime_ = 0;
  sampleCounter_ = 0;
}

void SSEServer::search(std::vector<std::string> &_return,
                       const std::string &stag,
                       const std::vector<std::string> &xtoken,
                       const std::string &logPath) {
  auto t1 = NowTime();
  //   phase-I
  //  std::cout << "Server search g: " << Element2Str(this->g_) << std::endl;
  std::vector<tSetTuple> tuples = TSet_[stag];

  for (int i = 0; i < tuples.size(); i++) {
    std::string xind_z_inv = tuples[i].y;
    // !!!!!!!!!!!!!git rid of padding 0, cause element string representation
    // won't contain '\0'
    xind_z_inv = StrStrip(xind_z_inv);
    element_t e_xind_z_inv;
    element_init_Zr(e_xind_z_inv, pairing_);
    int flag1 = Str2Element(e_xind_z_inv, xind_z_inv, 0);

    std::string g_xtrap_z = xtoken[i];
    element_t e_g_xtrap_z;
    element_init_G1(e_g_xtrap_z, pairing_);
    int flag2 = Str2Element(e_g_xtrap_z, g_xtrap_z, 1);
    // std::cout << "g_xtrap_z: " << Element2Str(e_g_xtrap_z) << endl;
    element_t xtag;
    element_init_G1(xtag, pairing_);
    if (flag1 == 1 && flag2 == 1) {
      element_pow_zn(xtag, e_g_xtrap_z, e_xind_z_inv);
      std::string xtagStr = Element2Str(xtag);

      // use bloom filter
      if (XSet_->Contain(xtagStr) == bloomfilter::Ok) {
        _return.emplace_back(tuples[i].encID);
      }

    } else { // simulate
      element_t e1;
      element_t e2;
      element_init_Zr(e1, pairing_);
      element_init_G1(e2, pairing_);

      element_set_str(e1, "79735592625165692018661032316396977787955663612",
                      10);
      element_set_str(e2,
                      "[283212563081518405826628229250166693004282940978,"
                      "252386776103601319725751372526848782567166659871]",
                      10);
      element_pow_zn(xtag, e2, e1);
      std::string xtagStr = Element2Str(xtag);
      XSet_->Contain(xtagStr);
    }
  }

  auto t2 = NowTime();

  serverTime_ += t2 - t1;
  repeatCounter_++;
  if (repeatCounter_ == repeatTimes && logPath != "") {
    std::ofstream out_(benchmarkPath + logPath, std::ios::app);
    if (!out_) {
      std::cerr << "Server: Failed to open the file: " << logPath << std::endl;
      return;
    }

    // wirite
    std::cout << "Server Time: " << serverTime_ / repeatTimes << std::endl;
    if (sampleCounter_ == 0) {
      out_ << "[";
    }

    out_ << serverTime_ / repeatTimes << ", ";
    repeatCounter_ = 0;
    serverTime_ = 0;
    sampleCounter_++;
    if (sampleCounter_ == samplePointNums) {
      out_ << "]"
           << "\n";
      sampleCounter_ = 0;
    }
    out_.close();
  }
}

} // namespace OXT