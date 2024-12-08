//
// Created by shangqi on 2020/6/20.
//

#ifndef GGMNODE_H
#define GGMNODE_H
#include "CryptoTools.h"
#include <cstring>
class GGMNode {
public:
  long index_;
  int level_;
  std::string key_ = std::string(PRF_SIZE, '0');

  GGMNode(long index, int level) {
    this->index_ = index;
    this->level_ = level;
  }

  GGMNode(long index, int level, std::string key) {
    this->index_ = index;
    this->level_ = level;
    this->key_ = key;
  }
};

#endif // GGMNODE_H
