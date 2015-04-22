// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/tools/quic/quic_server_session.h"

#include "base/logging.h"
#include "net/quic/quic_connection.h"
#include "net/quic/quic_flags.h"

namespace net {
namespace tools {

QuicServerSession::QuicServerSession(const QuicConfig& config,
                                     QuicConnection* connection,
                                     QuicServerSessionVisitor* visitor)
    : QuicSession(connection, config),
      visitor_(visitor),
      bandwidth_estimate_sent_to_client_(QuicBandwidth::Zero()),
      last_scup_time_(QuicTime::Zero()),
      last_scup_sequence_number_(0) {}

QuicServerSession::~QuicServerSession() {}

void QuicServerSession::InitializeSession(
    const QuicCryptoServerConfig* crypto_config) {
  QuicSession::InitializeSession();
  crypto_stream_.reset(CreateQuicCryptoServerStream(crypto_config));
}

QuicCryptoServerStream* QuicServerSession::CreateQuicCryptoServerStream(
    const QuicCryptoServerConfig* crypto_config) {
  return new QuicCryptoServerStream(*crypto_config, this);
}

void QuicServerSession::OnConnectionClosed(QuicErrorCode error,
                                           bool from_peer) {
  QuicSession::OnConnectionClosed(error, from_peer);
  // In the unlikely event we get a connection close while doing an asynchronous
  // crypto event, make sure we cancel the callback.
  if (crypto_stream_.get() != nullptr) {
    crypto_stream_->CancelOutstandingCallbacks();
  }
  visitor_->OnConnectionClosed(connection()->connection_id(), error);
}

void QuicServerSession::OnWriteBlocked() {
  QuicSession::OnWriteBlocked();
  visitor_->OnWriteBlocked(connection());
}

bool QuicServerSession::ShouldCreateIncomingDataStream(QuicStreamId id) {
  if (id % 2 == 0) {
    DVLOG(1) << "Invalid incoming even stream_id:" << id;
    connection()->SendConnectionClose(QUIC_INVALID_STREAM_ID);
    return false;
  }
  if (GetNumOpenStreams() >= get_max_open_streams()) {
    DVLOG(1) << "Failed to create a new incoming stream with id:" << id
             << " Already " << GetNumOpenStreams() << " streams open (max "
             << get_max_open_streams() << ").";
    connection()->SendConnectionClose(QUIC_TOO_MANY_OPEN_STREAMS);
    return false;
  }
  return true;
}

QuicDataStream* QuicServerSession::CreateIncomingDataStream(
    QuicStreamId id) {
  return nullptr;
}

QuicDataStream* QuicServerSession::CreateOutgoingDataStream() {
  DLOG(ERROR) << "Server push not yet supported";
  return nullptr;
}

QuicCryptoServerStream* QuicServerSession::GetCryptoStream() {
  return crypto_stream_.get();
}

}  // namespace tools
}  // namespace net
