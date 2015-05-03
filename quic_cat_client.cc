// Small demo that reads form stdin and sents over a quic connection

#include <iostream>

#include "net/base/ip_endpoint.h"
#include "net/tools/quic/quic_client.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/base/privacy_mode.h"
#include "net/quic/quic_protocol.h"
#include "net/quic/quic_server_id.h"
#include "net/quic/quic_utils.h"

using namespace std;

int main(int argc, char *argv[]) {

  // Is needed for whatever reason
  base::AtExitManager exit_manager;

  net::IPAddressNumber ip_address = (net::IPAddressNumber) std::vector<unsigned char> { 127, 0, 0, 1};
  net::IPEndPoint server_address(ip_address, 1337);
  net::QuicServerId server_id("localhost", 1337, /*is_http*/ false, net::PRIVACY_MODE_DISABLED);
  net::QuicVersionVector supported_versions = net::QuicSupportedVersions();
  net::EpollServer epoll_server;
  
  net::tools::QuicClient client(server_address, server_id, supported_versions, &epoll_server);
  if (!client.Initialize()) {
    cerr << "Could not initialize client" << endl;
    return 1;
  }
  cout << "Successfully initialized client" << endl;
  if (!client.Connect()) {
    cout << "Client could not connect" << endl;
    return 1;
  }
  cout << "Successfully connected to server, hopefully" << endl;

  net::tools::QuicClientStream* stream = client.CreateClientStream();

  for (string input; getline(cin, input);) {
    cout << "Sending data\n";
    stream->WriteStringPiece(base::StringPiece(input), false);
  }
}
