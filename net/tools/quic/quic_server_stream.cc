#include <iostream>

#include "net/tools/quic/quic_server_stream.h"

namespace net {
  namespace tools {

    QuicServerStream::QuicServerStream(QuicStreamId id, QuicSession* session, QuicConnectionHelperInterface* helper)
      : QuicDataStream(id, session),
        helper_(helper) {
      sequencer()->FlushBufferedFrames();
    }

    QuicServerStream::~QuicServerStream() {
      alarm_->Cancel();
    }

    uint32 QuicServerStream::ProcessRawData(const char* data, uint32 data_len) {
      bytes_received += data_len;
      std::cout << "Received " << data_len << " characters\n";
      std::cout << (HasBufferedData() ? "Stream has buffered data\n" : "Stream does not have buffered data\n");
      WriteStringPiece(base::StringPiece(data), false);
      return data_len;
    }

    QuicPriority QuicServerStream::EffectivePriority() const {
      return (QuicPriority) 3;
    }
    
    void QuicServerStream::WriteStringPiece(base::StringPiece data, bool fin) {
      this->WriteOrBufferData(data, fin, nullptr);
    }

    void QuicServerStream::SetupPerformanceAlarm() {
      alarm_ = helper_->CreateAlarm(this);
      QuicTime onesecond = helper_->GetClock()->ApproximateNow().Add(QuicTime::Delta::FromSeconds(1));
      alarm_->Set(onesecond);
    }

    QuicTime QuicServerStream::OnAlarm() {
      std::cout << "Stream " << id() << " has " << bytes_received << " bytes received\n";
      return helper_->GetClock()->ApproximateNow().Add(QuicTime::Delta::FromSeconds(1));
    }
  }
}
