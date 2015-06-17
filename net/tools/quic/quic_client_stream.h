#include "net/quic/quic_data_stream.h"

namespace net {
  namespace tools {

    class QuicClientStream: public QuicDataStream {
    public:
      QuicClientStream(QuicStreamId id, QuicSession* session);
      ~QuicClientStream();

      uint32 ProcessData(const char* data, uint32 data_len);

      void WriteStringPiece(base::StringPiece data, bool fin);
    };
  }
}
