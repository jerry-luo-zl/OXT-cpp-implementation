#pragma once
#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
static int repeatTimes = 30;
static const int samplePointNums = 7;

// config for mysql database
static const std::string dbName = "wikitest";
static const std::string tabName = "page_exp_cut";
static const char *host_ = "127.0.0.1";
static const char *user_ = "root";
static const char *pwd_ = "lzl_pass";
static const unsigned int port_ = 3306;

static const std::string RootDir = "/Volumes/backup/VHSSE/NewWork/";
static const std::string EDBRootDir = RootDir + "EDB/";
static const std::string benchmarkPath = RootDir + "benchmark/";

#endif