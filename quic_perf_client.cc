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
#include "net/quic/quic_protocol.h"
#include "net/quic/quic_server_id.h"
#include "net/quic/quic_utils.h"

using namespace std;

uint64 FLAGS_total_transfer = 10 * 1000 * 1000;
uint64 FLAGS_chunk_size = 1000;

string randomString(uint length) {
  string result = "";
  for (uint i = 0; i < length; i++) {
    result.push_back('a' + rand()%26);
  }
  return result;
}

int main(int argc, char *argv[]) {
  base::CommandLine::Init(argc, argv);
  base::CommandLine* line = base::CommandLine::ForCurrentProcess();

  if (line->HasSwitch("t")) {
    if (!base::StringToUint64(line->GetSwitchValueASCII("t"), &FLAGS_total_transfer)) {
      cout << "-t must be an unsigned integer\n";
      return 1;
    }
  }
  if (line->HasSwitch("c")) {
    if (!base::StringToUint64(line->GetSwitchValueASCII("c"), &FLAGS_chunk_size)) {
      cout << "-c must be an unsigned integer\n";
      return 1;
    }
  }

  cout << "Run parameters are:\nchunk size: " << FLAGS_chunk_size << "\ntotal size: " << FLAGS_total_transfer << "\n";

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
  string out;
  for (uint64 i = 0; i < FLAGS_total_transfer; i += FLAGS_chunk_size) {
    out = randomString(FLAGS_chunk_size);
    //cout << "Sending " << out << "\n";
    stream->WriteStringPiece(base::StringPiece(out), false);
    cout << (stream->HasBufferedData() ? "Stream has data buffered\n" : "Stream has no data buffered\n");
    cout << (stream->flow_controller()->IsBlocked() ? "Flow controller is blocked\n" : "Flow controller is not blocked\n");
    if (stream->HasBufferedData()) {
      client.WaitForEvents();
    }
  }

  while (stream->HasBufferedData()) {
    client.WaitForEvents();
  }

  stream->CloseConnection(net::QUIC_NO_ERROR);
  client.Disconnect();
}

