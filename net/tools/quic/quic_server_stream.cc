#include <iostream>

#include "net/tools/quic/quic_server_stream.h"
#include "net/quic/quic_time.h"

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
      //WriteStringPiece(base::StringPiece(data), false);
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
      QuicWallTime now = helper_->GetClock()->WallNow();
      std::cout << bytes_received << "\n";
      return helper_->GetClock()->ApproximateNow().Add(QuicTime::Delta::FromSeconds(1));
    }

    void QuicServerStream::OnClose() {
      QuicDataStream::OnClose();
      exit(0);
    }
  }
}
