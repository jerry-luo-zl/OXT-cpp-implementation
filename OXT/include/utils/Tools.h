#pragma once
#ifndef TOOLS_H_
#define TOOLS_H_

#include "Config.h"
#include <bitset>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <gmp.h>
#include <iomanip>
#include <iostream>
#include <mysql.h>
#include <pbc/pbc.h>
#include <pbc/pbc_field.h>
#include <sstream>

extern uint64_t S1_clientSearchTime_I;
extern uint64_t S1_clientSearchTime_II;

extern uint64_t S2_clientSearchTime;
extern uint64_t S3_clientSearchTime;

static void InitConn(MYSQL *conn_, const char *dbName) {
  mysql_options(conn_, MYSQL_SET_CHARSET_NAME, "utf8");
  if (!mysql_real_connect(conn_, host_, user_, pwd_, dbName, port_, NULL, 0)) {
    std::cout << "Failed to connect DB" << std::endl;
    //  throw exception
    exit(1);
  }
}

static MYSQL_RES *ExecuteSql(MYSQL *conn_, std::string sql) {

  // get num of distinct values in sterm
  if (mysql_query(conn_, sql.c_str())) {
    fprintf(stderr, "Failed to select data!!!Error:%s\n", mysql_error(conn_));
  }
  MYSQL_RES *res = mysql_store_result(conn_);
  return res;
}

static std::string StrToBin(const std::string &str) {
  std::string bin;
  for (char c : str) {
    std::bitset<8> bits(static_cast<int>(c));
    bin += bits.to_string();
  }
  return bin;
}

static std::string StrToHex(const std::string &str) {
  std::stringstream ss;
  for (unsigned char c : str) {
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
  }
  return ss.str();
}

static std::string StrStrip(std::string s) {
  std::string f = s;
  size_t pos = f.find_last_not_of('\0');
  if (pos != std::string::npos) {
    f.erase(pos + 1);
  }
  return f;
}

static std::uint64_t NowTime() {
  return ::std::chrono::duration_cast<::std::chrono::nanoseconds>(
             ::std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

template <typename T> static void writeLog(std::ofstream out, T val) {
  if (!out) {
    std::cerr << "Failed to open the file." << std::endl;
    return;
  }

  out << val << "\n" << std::endl;
}

template <typename T> static void readLog(std::ofstream in, T &val) {
  if (!in) {
    std::cerr << "Failed to open the file." << std::endl;
    return;
  }
  in >> val;
}

static std::string Element2Str(element_t e) {
  char *s = new char[105];
  memset(s, 0, 105);
  int len = element_snprint(s, 105, e);
  // std::cout << len << std::endl;
  std::string str = std::string(s, 105);
  delete[] s;
  return str;
}

static int Str2Element(element_t &e, std::string str, int sign) {
  char s[105];
  memset(s, 0, 105);
  memcpy(s, str.c_str(), str.size());

  if (element_set_str(e, s, 10) == 0) { // fail
    return 0;
  } else {
    return 1;
  }
}
#endif