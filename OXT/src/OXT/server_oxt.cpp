#include <OXT/SSEServer.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TServerSocket.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace ::OXT;

int main(int argc, char **argv) {

  // change to remote server
  TSimpleServer server(
      std::make_shared<SSEServiceProcessor>(std::make_shared<SSEServer>()),
      std::make_shared<TServerSocket>("127.0.0.1", 8080),
      std::make_shared<TFramedTransportFactory>(),
      std::make_shared<TBinaryProtocolFactory>());
  // start service
  server.serve();

  return 0;
}