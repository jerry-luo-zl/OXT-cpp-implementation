#include "OXT/SSEClient.h"
#include "filter/bloom.h"
#include "pbc/pbc_field.h"
#include "utils/CompressBF.h"
#include "utils/CryptoTools.h"
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace OXT {

SSEClient::SSEClient() {

  tSetPath_ = EDBRootDir + "OXT_TSet.txt";
  xSetPath_ = EDBRootDir + "OXT_XSet.txt";
  counterPath_ = EDBRootDir + "OXT_counter.txt";
  gPath_ = EDBRootDir + "OXT_g.txt";

  conn_ = mysql_init(NULL);
  tabName_ = tabName;
  InitConn(conn_, dbName.c_str());
  // get num of distinct values in sterm
  MYSQL_RES *res = ExecuteSql(conn_, "select count(*) from " + tabName_);
  int recNum = std::stoi(std::string(mysql_fetch_row(res)[0]));

  res = ExecuteSql(conn_, "select * from " + tabName_);
  MYSQL_FIELD *fields = mysql_fetch_fields(res);
  columnCount_ = mysql_num_fields(res);
  for (int i = 0; i < columnCount_; i++) {
    fieldNames.emplace_back(std::string(fields[i].name));
    std::cout << std::string(fields[i].name) << ", "
              << std::string(fields[i].name).size() << std::endl;
  }

  XSet_ = new BloomFilter(2 * recNum, 16);

  // initialize curve
  char param[1024];
  FILE *file = fopen((EDBRootDir + "d159.param").c_str(), "r");
  size_t count = fread(param, 1, 1024, file);
  fclose(file);
  if (!count)
    pbc_die("input error");
  pairing_init_set_buf(pairing_, param, count);

  // init g_
  element_init_G1(g_, pairing_);
  if (!std::filesystem::exists(gPath_)) {
    element_random(g_);
    // std::cout << "Client EDBSetup g: " << Element2Str(this->g_) << std::endl;
  } else {
    std::ifstream f(gPath_);
    if (f.is_open()) {
      std::ostringstream oss;
      oss << f.rdbuf();
      std::string gStr = oss.str();
      f.close();
      int ret = element_set_str(g_, gStr.c_str(), 10);
      if (ret == 0) {
        std::cout << "client read g_ fail!" << std::endl; // exception
        exit(0);
      }
    }
    f.close();
  }
}

void SSEClient::EDBSetup() {
  MYSQL_RES *res = ExecuteSql(conn_, "SELECT * FROM " + tabName_);
  MYSQL_ROW row;
  std::vector<std::string> xSetEntries;

  element_t e_xind;
  element_t e_z;
  element_t z_inv;
  element_t y;
  element_t e_xtrap;
  element_t xtrap_xind;
  element_t xtag;
  element_init_Zr(e_xind, pairing_);
  element_init_Zr(e_z, pairing_);
  element_init_Zr(z_inv, pairing_);
  element_init_Zr(y, pairing_);
  element_init_Zr(e_xtrap, pairing_);
  element_init_G1(xtag, pairing_);
  element_init_Zr(xtrap_xind, pairing_);
  // row consists of two three column: id, s-term, x-term
  // row[0] : page_id, row[1] : page_len, row[2] : page_namespace
  // x-term : page_len, s-term : page_namespace
  while ((row = mysql_fetch_row(res))) {
    std::string recId(row[0]);
    for (int i = 1; i < columnCount_; i++) {
      std::string w = fieldNames[i] + "," + std::string(row[i]);

      // TSet index
      std::string Fw = CryptoTools::Prf(w, K0_);

      // gen enc id
      std::string encID = CryptoTools::AesEncrypt(Kenc_, iv_, recId);

      // gen y=xind*z^-1
      if (!counter_.count(Fw)) {
        counter_[Fw] = 0;
      }
      std::string xind = CryptoTools::Prf(recId, KI_);
      std::string xz =
          CryptoTools::Prf(w + ":" + std::to_string(counter_[Fw]), KZ_);
      element_from_hash(e_xind, (void *)(&xind[0]), xind.size());
      element_from_hash(e_z, (void *)&xz[0], xz.size());
      element_invert(z_inv, e_z);
      element_mul(y, e_xind, z_inv);

      std::string yStr = Element2Str(y);
      counter_[Fw] = counter_[Fw] + 1;

      // append (encID, y) into TSet
      TSet_[Fw].emplace_back(tSetTuple(encID, yStr));

      // gen XSet entry
      std::string xtrap = CryptoTools::Prf(w, KX_);
      element_from_hash(e_xtrap, (void *)&xtrap[0], xtrap.size());
      element_mul(xtrap_xind, e_xtrap, e_xind);
      element_pow_zn(xtag, g_, xtrap_xind);
      std::string xtagStr = Element2Str(xtag);

      xSetEntries.emplace_back(xtagStr);
    }
  }
  element_clear(e_xind);
  element_clear(e_z);
  element_clear(z_inv);
  element_clear(y);
  element_clear(e_xtrap);
  element_clear(xtrap_xind);
  element_clear(xtag);

  (*XSet_).AddAll(xSetEntries);
  SerializeEDB();
  mysql_close(conn_);
  // serverTest_ = new SSEServer(TSet_, XSet_, Element2Str(this->g_));
}

void SSEClient::SerializeEDB() {
  std::ofstream out1(tSetPath_);
  int s = TSet_.size();
  out1.write(reinterpret_cast<const char *>(&s), sizeof(s));
  for (auto it = TSet_.begin(); it != TSet_.end(); it++) {
    int a = it->first.size();
    out1 << it->first;
    int b = counter_[it->first];
    out1.write(reinterpret_cast<const char *>(&b), sizeof(b));
    for (int i = 0; i < it->second.size(); i++) {
      int c = (it->second)[i].encID.size();
      out1.write(reinterpret_cast<const char *>(&c), sizeof(c));

      out1 << (it->second)[i].encID;

      int d = (it->second)[i].y.size();
      out1.write(reinterpret_cast<const char *>(&d), sizeof(d));

      out1 << (it->second)[i].y;
    }
  }
  out1.close();

  std::ofstream out2(xSetPath_);
  // compress BF
  CompressedBF cbf = XSet_->Compress();
  out2.write(reinterpret_cast<const char *>(&(cbf.size)), sizeof(cbf.size));
  out2.write(reinterpret_cast<const char *>(&(cbf.bits_per_item)),
             sizeof(cbf.bits_per_item));
  out2.write(reinterpret_cast<const char *>(&(cbf.seed)), sizeof(cbf.seed));
  int e = cbf.data.size();
  out2.write(reinterpret_cast<const char *>(&e), sizeof(e));
  for (int i = 0; i < cbf.data.size(); i++) {
    int64_t tmp = cbf.data[i];
    out2.write(reinterpret_cast<const char *>(&tmp), sizeof(tmp));
  }
  out2.close();

  // store counter
  std::ofstream out3(counterPath_);
  int counterSize = counter_.size();
  out3.write(reinterpret_cast<const char *>(&counterSize), sizeof(counterSize));
  // compress BF
  for (auto it = counter_.begin(); it != counter_.end(); it++) {
    int s = it->first.size();
    out3.write(reinterpret_cast<const char *>(&s), sizeof(s));
    out3 << it->first;
    out3.write(reinterpret_cast<const char *>(&(it->second)),
               sizeof(it->second));
  }
  out3.close();

  // wirte g_
  std::string gStr = Element2Str(this->g_);
  std::ofstream f(gPath_);
  if (!f.is_open()) {
    std::cerr << "open file failed:" << gPath_ << std::endl;
    exit(0);
  }
  f << gStr;
  f.close();
}

void SSEClient::ReadCounter() {
  if (!std::filesystem::exists(counterPath_)) {
    return;
  } else {
    std::ifstream in(counterPath_);
    int counterSize;
    in.read(reinterpret_cast<char *>(&counterSize), sizeof(counterSize));

    for (int i = 0; i < counterSize; i++) {
      int FwSize;
      in.read(reinterpret_cast<char *>(&FwSize), sizeof(FwSize));

      char buf[FwSize];
      in.read(buf, FwSize);
      std::string Fw = std::string(buf, FwSize);

      int cnt;
      in.read(reinterpret_cast<char *>(&cnt), sizeof(cnt));
      counter_[Fw] = cnt;
    }
    in.close();
  }
}

void SSEClient::Search(std::vector<std::string> &res, const std::string sterm,
                       const std::string xterm, std::string logPath) {

  std::shared_ptr<TTransport> socket(new TSocket("127.0.0.1", 8080));
  transport_ = std::shared_ptr<TTransport>(new TFramedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport_));
  server_ = new SSEServiceClient(protocol);
  transport_->open();
  // std::cout << "connection established!" << std::endl;
  // std::cout << "Client search g: " << Element2Str(this->g_) << std::endl;

  // gen stag
  std::string stag;
  stag = CryptoTools::Prf(sterm, K0_);

  // gen xtoken
  std::vector<std::string> xtokens;
  std::string xtrap = CryptoTools::Prf(xterm, KX_);
  element_t e_xtrap;
  element_init_Zr(e_xtrap, pairing_);
  element_from_hash(e_xtrap, (void *)&xtrap[0], xtrap.size());
  int t = counter_[stag];
  for (int i = 0; i < counter_[stag]; i++) {
    std::string xz = CryptoTools::Prf(sterm + ":" + std::to_string(i), KZ_);
    element_t e_z;
    element_init_Zr(e_z, pairing_);
    element_from_hash(e_z, (void *)&xz[0], xz.size());

    element_t xtoken;
    element_t xtrap_z;
    element_init_Zr(xtrap_z, pairing_);
    element_init_G1(xtoken, pairing_);
    element_mul(xtrap_z, e_xtrap, e_z);
    element_pow_zn(xtoken, g_, xtrap_z);
    xtokens.emplace_back(Element2Str(xtoken));
  }

  std::vector<std::string> rawRes;
  server_->search(rawRes, stag, xtokens, logPath);
  // this->searchTest(rawRes, stag, xtokens, logPath);

  // decryption
  /*
  for (auto s : tmp) {
    std::cout << StrToHex(s) << std::endl;
  }
  */
  for (auto s : rawRes) {
    std::string ptext = CryptoTools::AesDecrypt(Kenc_, iv_, s);
    if (ptext.size() == 0) {
      ptext = "0000000\t\t";
    }
    // get rid of '\t'
    ptext.erase(std::remove(ptext.begin(), ptext.end(), '\t'), ptext.end());
    int id;
    if (!CryptoTools::IsValidId(ptext))
      continue;
    try {
      id = std::stoi(ptext);
    } catch (std::invalid_argument) {
      continue;
    }
    if (id > 0) {
      res.emplace_back(std::to_string(id));
    }
  }

  transport_->close();
}

void SSEClient::searchTest(std::vector<std::string> &_return,
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
}
} // namespace OXT