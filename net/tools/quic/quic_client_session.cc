// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/tools/quic/quic_client_session.h"

#include "base/logging.h"
#include "net/quic/crypto/crypto_protocol.h"
#include "net/quic/quic_server_id.h"

using std::string;

namespace net {
namespace tools {

QuicClientSession::QuicClientSession(const QuicConfig& config,
                                     QuicConnection* connection)
    : QuicClientSessionBase(connection, config), respect_goaway_(true) {
}

QuicClientSession::~QuicClientSession() {
}

void QuicClientSession::InitializeSession(
    const QuicServerId& server_id,
    QuicCryptoClientConfig* crypto_config) {
  crypto_stream_.reset(
      new QuicCryptoClientStream(server_id, this, nullptr, crypto_config));
  QuicClientSessionBase::InitializeSession();
}

void QuicClientSession::OnProofValid(
    const QuicCryptoClientConfig::CachedState& /*cached*/) {}

void QuicClientSession::OnProofVerifyDetailsAvailable(
    const ProofVerifyDetails& /*verify_details*/) {}

QuicCryptoClientStream* QuicClientSession::GetCryptoStream() {
  return crypto_stream_.get();
}

void QuicClientSession::CryptoConnect() {
  DCHECK(flow_controller());
  crypto_stream_->CryptoConnect();
}

int QuicClientSession::GetNumSentClientHellos() const {
  return crypto_stream_->num_sent_client_hellos();
}

QuicClientStream* QuicClientSession::CreateIncomingDataStream(
    QuicStreamId id) {
  DLOG(ERROR) << "Server push not supported";
  return nullptr;
}

QuicClientStream* QuicClientSession::CreateOutgoingDataStream() {
  QuicClientStream* stream = new QuicClientStream(GetNextStreamId(), this);
  return stream;
}

}  // namespace tools
}  // namespace net
