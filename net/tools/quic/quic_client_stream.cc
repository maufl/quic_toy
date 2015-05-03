#include "net/tools/quic/quic_client_stream.h"

namespace net {
  namespace tools {

    QuicClientStream::QuicClientStream(QuicStreamId id, QuicSession* session)
      : QuicDataStream(id, session) {
    }

    QuicClientStream::~QuicClientStream() {}

    uint32 QuicClientStream::ProcessData(const char* data, uint32 data_len) {
      return 0;
    }

    void QuicClientStream::WriteStringPiece(base::StringPiece data, bool fin) {
      this->WriteOrBufferData(data, fin, nullptr);
    }
  }
}
