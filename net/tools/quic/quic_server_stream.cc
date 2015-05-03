#include <iostream>

#include "net/tools/quic/quic_server_stream.h"

namespace net {
  namespace tools {

    QuicServerStream::QuicServerStream(QuicStreamId id, QuicSession* session)
      : QuicDataStream(id, session) {
    }

    QuicServerStream::~QuicServerStream() {}

    uint32 QuicServerStream::ProcessRawData(const char* data, uint32 data_len) {
      std::cout << data << "\n";
      return data_len;
    }

    QuicPriority QuicServerStream::EffectivePriority() const {
      return (QuicPriority) 3;
    }
    
    void QuicServerStream::WriteStringPiece(base::StringPiece data, bool fin) {
      this->WriteOrBufferData(data, fin, nullptr);
    }
  }
}
