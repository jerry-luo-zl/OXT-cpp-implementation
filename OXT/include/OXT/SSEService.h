/**
 * Autogenerated by Thrift Compiler (0.20.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef OXT_SSEService_H
#define OXT_SSEService_H

#include "Server_types.h"
#include <memory>
#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>

namespace OXT {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inheriting methods via dominance
#endif

class SSEServiceIf {
public:
  virtual ~SSEServiceIf() {}
  virtual void search(std::vector<std::string> &_return,
                      const std::string &stag,
                      const std::vector<std::string> &xtoken,
                      const std::string &logPath) = 0;
};

class SSEServiceIfFactory {
public:
  typedef SSEServiceIf Handler;

  virtual ~SSEServiceIfFactory() {}

  virtual SSEServiceIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) = 0;
  virtual void releaseHandler(SSEServiceIf * /* handler */) = 0;
};

class SSEServiceIfSingletonFactory : virtual public SSEServiceIfFactory {
public:
  SSEServiceIfSingletonFactory(const ::std::shared_ptr<SSEServiceIf> &iface)
      : iface_(iface) {}
  virtual ~SSEServiceIfSingletonFactory() {}

  virtual SSEServiceIf *
  getHandler(const ::apache::thrift::TConnectionInfo &) override {
    return iface_.get();
  }
  virtual void releaseHandler(SSEServiceIf * /* handler */) override {}

protected:
  ::std::shared_ptr<SSEServiceIf> iface_;
};

class SSEServiceNull : virtual public SSEServiceIf {
public:
  virtual ~SSEServiceNull() {}
  void search(std::vector<std::string> & /* _return */,
              const std::string & /* stag */,
              const std::vector<std::string> & /* xtoken */,
              const std::string & /* logPath */) override {
    return;
  }
};

typedef struct _SSEService_search_args__isset {
  _SSEService_search_args__isset()
      : stag(false), xtoken(false), logPath(false) {}
  bool stag : 1;
  bool xtoken : 1;
  bool logPath : 1;
} _SSEService_search_args__isset;

class SSEService_search_args {
public:
  SSEService_search_args(const SSEService_search_args &);
  SSEService_search_args &operator=(const SSEService_search_args &);
  SSEService_search_args() noexcept : stag(), logPath() {}

  virtual ~SSEService_search_args() noexcept;
  std::string stag;
  std::vector<std::string> xtoken;
  std::string logPath;

  _SSEService_search_args__isset __isset;

  void __set_stag(const std::string &val);

  void __set_xtoken(const std::vector<std::string> &val);

  void __set_logPath(const std::string &val);

  bool operator==(const SSEService_search_args &rhs) const {
    if (!(stag == rhs.stag))
      return false;
    if (!(xtoken == rhs.xtoken))
      return false;
    if (!(logPath == rhs.logPath))
      return false;
    return true;
  }
  bool operator!=(const SSEService_search_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator<(const SSEService_search_args &) const;

  uint32_t read(::apache::thrift::protocol::TProtocol *iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol *oprot) const;
};

class SSEService_search_pargs {
public:
  virtual ~SSEService_search_pargs() noexcept;
  const std::string *stag;
  const std::vector<std::string> *xtoken;
  const std::string *logPath;

  uint32_t write(::apache::thrift::protocol::TProtocol *oprot) const;
};

typedef struct _SSEService_search_result__isset {
  _SSEService_search_result__isset() : success(false) {}
  bool success : 1;
} _SSEService_search_result__isset;

class SSEService_search_result {
public:
  SSEService_search_result(const SSEService_search_result &);
  SSEService_search_result &operator=(const SSEService_search_result &);
  SSEService_search_result() noexcept {}

  virtual ~SSEService_search_result() noexcept;
  std::vector<std::string> success;

  _SSEService_search_result__isset __isset;

  void __set_success(const std::vector<std::string> &val);

  bool operator==(const SSEService_search_result &rhs) const {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator!=(const SSEService_search_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator<(const SSEService_search_result &) const;

  uint32_t read(::apache::thrift::protocol::TProtocol *iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol *oprot) const;
};

typedef struct _SSEService_search_presult__isset {
  _SSEService_search_presult__isset() : success(false) {}
  bool success : 1;
} _SSEService_search_presult__isset;

class SSEService_search_presult {
public:
  virtual ~SSEService_search_presult() noexcept;
  std::vector<std::string> *success;

  _SSEService_search_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol *iprot);
};

class SSEServiceClient : virtual public SSEServiceIf {
public:
  SSEServiceClient(
      std::shared_ptr<::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  SSEServiceClient(
      std::shared_ptr<::apache::thrift::protocol::TProtocol> iprot,
      std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot, oprot);
  }

private:
  void
  setProtocol(std::shared_ptr<::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot, prot);
  }
  void
  setProtocol(std::shared_ptr<::apache::thrift::protocol::TProtocol> iprot,
              std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot) {
    piprot_ = iprot;
    poprot_ = oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }

public:
  std::shared_ptr<::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr<::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void search(std::vector<std::string> &_return, const std::string &stag,
              const std::vector<std::string> &xtoken,
              const std::string &logPath) override;
  void send_search(const std::string &stag,
                   const std::vector<std::string> &xtoken,
                   const std::string &logPath);
  void recv_search(std::vector<std::string> &_return);

protected:
  std::shared_ptr<::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr<::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol *iprot_;
  ::apache::thrift::protocol::TProtocol *oprot_;
};

class SSEServiceProcessor : public ::apache::thrift::TDispatchProcessor {
protected:
  ::std::shared_ptr<SSEServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol *iprot,
                            ::apache::thrift::protocol::TProtocol *oprot,
                            const std::string &fname, int32_t seqid,
                            void *callContext) override;

private:
  typedef void (SSEServiceProcessor::*ProcessFunction)(
      int32_t, ::apache::thrift::protocol::TProtocol *,
      ::apache::thrift::protocol::TProtocol *, void *);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_search(int32_t seqid,
                      ::apache::thrift::protocol::TProtocol *iprot,
                      ::apache::thrift::protocol::TProtocol *oprot,
                      void *callContext);

public:
  SSEServiceProcessor(::std::shared_ptr<SSEServiceIf> iface) : iface_(iface) {
    processMap_["search"] = &SSEServiceProcessor::process_search;
  }

  virtual ~SSEServiceProcessor() {}
};

class SSEServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
public:
  SSEServiceProcessorFactory(
      const ::std::shared_ptr<SSEServiceIfFactory> &handlerFactory) noexcept
      : handlerFactory_(handlerFactory) {}

  ::std::shared_ptr<::apache::thrift::TProcessor>
  getProcessor(const ::apache::thrift::TConnectionInfo &connInfo) override;

protected:
  ::std::shared_ptr<SSEServiceIfFactory> handlerFactory_;
};

class SSEServiceMultiface : virtual public SSEServiceIf {
public:
  SSEServiceMultiface(std::vector<std::shared_ptr<SSEServiceIf>> &ifaces)
      : ifaces_(ifaces) {}
  virtual ~SSEServiceMultiface() {}

protected:
  std::vector<std::shared_ptr<SSEServiceIf>> ifaces_;
  SSEServiceMultiface() {}
  void add(::std::shared_ptr<SSEServiceIf> iface) { ifaces_.push_back(iface); }

public:
  void search(std::vector<std::string> &_return, const std::string &stag,
              const std::vector<std::string> &xtoken,
              const std::string &logPath) override {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->search(_return, stag, xtoken, logPath);
    }
    ifaces_[i]->search(_return, stag, xtoken, logPath);
    return;
  }
};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class SSEServiceConcurrentClient : virtual public SSEServiceIf {
public:
  SSEServiceConcurrentClient(
      std::shared_ptr<::apache::thrift::protocol::TProtocol> prot,
      std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync)
      : sync_(sync) {
    setProtocol(prot);
  }
  SSEServiceConcurrentClient(
      std::shared_ptr<::apache::thrift::protocol::TProtocol> iprot,
      std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot,
      std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync)
      : sync_(sync) {
    setProtocol(iprot, oprot);
  }

private:
  void
  setProtocol(std::shared_ptr<::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot, prot);
  }
  void
  setProtocol(std::shared_ptr<::apache::thrift::protocol::TProtocol> iprot,
              std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot) {
    piprot_ = iprot;
    poprot_ = oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }

public:
  std::shared_ptr<::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr<::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void search(std::vector<std::string> &_return, const std::string &stag,
              const std::vector<std::string> &xtoken,
              const std::string &logPath) override;
  int32_t send_search(const std::string &stag,
                      const std::vector<std::string> &xtoken,
                      const std::string &logPath);
  void recv_search(std::vector<std::string> &_return, const int32_t seqid);

protected:
  std::shared_ptr<::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr<::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol *iprot_;
  ::apache::thrift::protocol::TProtocol *oprot_;
  std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace OXT

#endif
