#ifndef NET_TOOLS_QUIC_SERVER_STREAM_
#define NET_TOOLS_QUIC_SERVER_STREAM_

#include "net/quic/quic_protocol.h"
#include "net/quic/quic_data_stream.h"

namespace net {
  namespace tools {

    class QuicServerStream: public QuicDataStream {
    public:
      QuicServerStream(QuicStreamId id, QuicSession* session);
      ~QuicServerStream();

      uint32 ProcessRawData(const char* data, uint32 data_len) override;

      QuicPriority EffectivePriority() const override;

      void WriteStringPiece(base::StringPiece data, bool fin);

    };
  }
}

#endif
