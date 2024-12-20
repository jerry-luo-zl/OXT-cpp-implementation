/**
 * Autogenerated by Thrift Compiler (0.20.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "utils/CompressBF.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

CompressedBF::~CompressedBF() noexcept {}

void CompressedBF::__set_size(const int64_t val) { this->size = val; }

void CompressedBF::__set_bits_per_item(const int64_t val) {
  this->bits_per_item = val;
}

void CompressedBF::__set_seed(const int64_t val) { this->seed = val; }

void CompressedBF::__set_data(const std::vector<int64_t> &val) {
  this->data = val;
}
std::ostream &operator<<(std::ostream &out, const CompressedBF &obj) {
  obj.printTo(out);
  return out;
}

uint32_t CompressedBF::read(::apache::thrift::protocol::TProtocol *iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid) {
    case 1:
      if (ftype == ::apache::thrift::protocol::T_I64) {
        xfer += iprot->readI64(this->size);
        this->__isset.size = true;
      } else {
        xfer += iprot->skip(ftype);
      }
      break;
    case 2:
      if (ftype == ::apache::thrift::protocol::T_I64) {
        xfer += iprot->readI64(this->bits_per_item);
        this->__isset.bits_per_item = true;
      } else {
        xfer += iprot->skip(ftype);
      }
      break;
    case 3:
      if (ftype == ::apache::thrift::protocol::T_I64) {
        xfer += iprot->readI64(this->seed);
        this->__isset.seed = true;
      } else {
        xfer += iprot->skip(ftype);
      }
      break;
    case 4:
      if (ftype == ::apache::thrift::protocol::T_LIST) {
        {
          this->data.clear();
          uint32_t _size0;
          ::apache::thrift::protocol::TType _etype3;
          xfer += iprot->readListBegin(_etype3, _size0);
          this->data.resize(_size0);
          uint32_t _i4;
          for (_i4 = 0; _i4 < _size0; ++_i4) {
            xfer += iprot->readI64(this->data[_i4]);
          }
          xfer += iprot->readListEnd();
        }
        this->__isset.data = true;
      } else {
        xfer += iprot->skip(ftype);
      }
      break;
    default:
      xfer += iprot->skip(ftype);
      break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t
CompressedBF::write(::apache::thrift::protocol::TProtocol *oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("CompressedBF");

  xfer += oprot->writeFieldBegin("size", ::apache::thrift::protocol::T_I64, 1);
  xfer += oprot->writeI64(this->size);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("bits_per_item",
                                 ::apache::thrift::protocol::T_I64, 2);
  xfer += oprot->writeI64(this->bits_per_item);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("seed", ::apache::thrift::protocol::T_I64, 3);
  xfer += oprot->writeI64(this->seed);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("data", ::apache::thrift::protocol::T_LIST, 4);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_I64,
                                  static_cast<uint32_t>(this->data.size()));
    std::vector<int64_t>::const_iterator _iter5;
    for (_iter5 = this->data.begin(); _iter5 != this->data.end(); ++_iter5) {
      xfer += oprot->writeI64((*_iter5));
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(CompressedBF &a, CompressedBF &b) {
  using ::std::swap;
  swap(a.size, b.size);
  swap(a.bits_per_item, b.bits_per_item);
  swap(a.seed, b.seed);
  swap(a.data, b.data);
  swap(a.__isset, b.__isset);
}

CompressedBF::CompressedBF(const CompressedBF &other6) {
  size = other6.size;
  bits_per_item = other6.bits_per_item;
  seed = other6.seed;
  data = other6.data;
  __isset = other6.__isset;
}
CompressedBF &CompressedBF::operator=(const CompressedBF &other7) {
  size = other7.size;
  bits_per_item = other7.bits_per_item;
  seed = other7.seed;
  data = other7.data;
  __isset = other7.__isset;
  return *this;
}
void CompressedBF::printTo(std::ostream &out) const {
  using ::apache::thrift::to_string;
  out << "CompressedBF(";
  out << "size=" << to_string(size);
  out << ", "
      << "bits_per_item=" << to_string(bits_per_item);
  out << ", "
      << "seed=" << to_string(seed);
  out << ", "
      << "data=" << to_string(data);
  out << ")";
}