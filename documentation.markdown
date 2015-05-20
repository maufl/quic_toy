# QUIC Documentation
This documentation primarily tries to sum up the responsibilities of the different classes.

## Classes
There are various classes that implement different aspects of QUIC and often realize an interface which is defined in another class.

### QuicConnection
The QuicConnection class handles the framing for QUIC clients or servers.
It provides a SendStreamData method to send stream data which is called from QuicSession.
It in turn uses the QuicPacketGenerator to generate QuicFrames.
The QuicConnection also implements the QuicPacketGenerator::DelegateInterface and is assigned to a QuicPacketGenerator as a delegate.
The QuicPacketGenerator will then call the OnSerializedPacket method of QuicConnection.
In the end, the frames are then written to the underling connection in the WritePacketInner using a QuicPacketWriter.

### QuicSession
The QuicSession class is a base class from which a concrete session class has to inherit.
It primarily dispatches incoming data onto the correct QUIC stream.
It also owns the QuicConnection, which is used to send the data over the wire.
Therefore, it represents a QUIC connection, consisting of multiple streams and abstracting the real network connection.
The QUIC streams call the WritevData method to send data.
In turn, the QuicConnection will call the methods of the QuicConnectionVisitorInterface to notify the session of new packets or changes to the connection.

### ReliableQuicStream
The ReliableQuicStream class is the base class for QUIC stream implementations.
It defines the interface a QUIC stream class has to satisfy.
It also implements the base logic of streams such as flow control, sequencing of frames, handeling stream or connection reset or close and buffered data writing.
A full QUIC stream class then only has to implement ProcessRawData and EffectivePriority.

### QuicDataStream
The QuicDataStream implements a QUIC stream that transports a SPDY request.
It expects the headers to be delivered out of band in a dedicated header stream that is managed by the session.
The headers are dispatched to it by calling OnStreamHeaders, OnStreamHeadersPriority and OnStreamHeadersComplete.
On initialization it blocks the QuicStreamSequencer until all headers are recieved.

### QuicStreamSequencer
The QuicStreamSequencer buffers frames until they can be passed up to the next layer.
This includes checking for duplicate frames, ordering frames so data will be in order and checking for error conditions.

### QuicPacketCreator
The QuicPacketCreator deals with creating frames and packets.
It can buffer frames to create larger packets that consist of multiple frames and can also generate FEC packets for frames.
It is used by the QuicPacketGenerator to create packets.

### QuicPacketGenerator
The QuicPacketGenerator class is used by the QuicConnection to generate and send packets.
It uses the QuicPacketCreator to build frames and packets.
When a packet is ready, it calls the OnSerializedPacket method of its delegate.

### QuicFramer
The QuicFramer class parses and builds QUIC packets.
It receives data via the ProcessPacket method and calls the methods of the QuicFrameVisitorInterface to inform the QuicConnection about new packets.

### QuicHeadersStream
The QuicHeadersStream transports SPDY headers out of band for the QuicDataConnetion.

## Interfaces
Some classes take an instance of an interface instead of a concrete class.

### QuicPacketWriter
The PacketWriter interface defines the method which will be called by QuicConnection to send packets.
It also defines some helper methods to find out whether the socket is blocked or not.
These methods have to be implemented in the application using QUIC.

### QuicPacketGenerator::DelegateInterface
The QuicPacketGenerator::DelegateInterface defines the methods the QuicPacketGenerator will call when new packets are available.
It is implemented by QuicConnection.

### QuicFrameVisitorInterface
The QuicFrameVisitorInterface defines the methods the QuicFramer will call when new QUIC packets processed.
It is also implemented by QuicConnection.

### QuicConnectionHelperInterface
The QuicConnectionHelperInterface defines some methods the QuicConnection will use to obtain a clock, get a source for random values or set alarms.

### QuicConnectionVisitorInterface
The QuicConnectionVisitorInterface defines methods that are called by QuicConnection when frames are received or other interesting thins happen.
It is implemented by QuicSession.
The OnStreamFrame method of this interface is used to hand stream frames from the connection to the session.

## Write your own QUIC application
To write an application that uses QUIC, you will have to subclass some of the QUIC classes as well as creating some new classes.

### New classes
To facilitate the handling of all the QUIC classes, you will probably want to create classes that wrap the client and the server functionality.
Also, you need to create at least the QuicPacketWriter helper class.

#### QUIC client application
You will need a class that will create a session, connection and a packet writer and connects all those classes.
It must also read the network packages and dispatches them to the session.
First it creates a QuicPacketWriter(Factory) to pass to the connection, which will likely require creating a UDP socket.

    QuicPacketWriter* writer = new QuicDefaultPacketWriter(fd_);

    DummyPacketWriterFactory factory(writer);

Then it will create a new session and connection, passing the connection to the session.

    session_.reset(new QuicClientSession(
        config_,
        new QuicConnection((QuicConnectionId) QuicRandom::GetInstance()->RandUint64(), // the new connection id
                           server_address_, // the address to connect to
                           helper_.get(), // an instance of the ConnectionHelper
                           factory, // the factory that will return a packet writer
                           false, // owns writer
                           false, // is server
                           server_id_.is_https(), supported_versions_)));

It then has to read the network packages and call the ProcessUdpPacket method of the connection.

    int bytes_read = QuicSocketUtils::ReadPacket(
          fd_, buf, arraysize(buf),
          overflow_supported_ ? &packets_dropped_ : nullptr, &client_ip,
          &server_address);

    if (bytes_read < 0) {
      return false;
    }

    QuicEncryptedPacket packet(buf, bytes_read, false);

    IPEndPoint client_address(client_ip, client_address_.port());
    session_->connection()->ProcessUdpPacket(client_address, server_address, packet);
  
If you want to have async network communication and want to use the epoll server it also has to implement the EpollCallbackInterface.

#### QUIC server application
The server side is a bit more involved, as it has to handle multiple sessions with multiple connections.
Therefore, the server class usually uses a dispatcher class.
It will handle the reading of network packets similar to the client (using epoll).
Instead of handing it to the connection directly it will hand it to the dispatcher.

    int bytes_read =
        QuicSocketUtils::ReadPacket(fd, buf, arraysize(buf),
                                    packets_dropped,
                                    &server_ip, &client_address);

    if (bytes_read < 0) {
      return false;  // We failed to read.
    }

    QuicEncryptedPacket packet(buf, bytes_read, false);

    IPEndPoint server_address(server_ip, port);
    processor->ProcessPacket(server_address, client_address, packet);

The dispatcher uses a framer to parse the packets which in turn will call the dispatcher on reading frames.

    void QuicDispatcher::ProcessPacket(const IPEndPoint& server_address,
                                       const IPEndPoint& client_address,
                                       const QuicEncryptedPacket& packet) {
      current_server_address_ = server_address;
      current_client_address_ = client_address;
      current_packet_ = &packet;
      // ProcessPacket will cause the packet to be dispatched in
      // OnUnauthenticatedPublicHeader, or sent to the time wait list manager
      // in OnAuthenticatedHeader.
      framer_.ProcessPacket(packet);

When the dispatcher has determined the connection to which the packet belongs it will call the ProcessUdpPacket of the sessions connection.
If there is no such session, the dispatcher will first create it.

    SessionMap::iterator it = session_map_.find(connection_id);
    if (it == session_map_.end()) {
      session = AdditionalValidityChecksThenCreateSession(header, connection_id);
      if (session == nullptr) {
        return false;
      }
    } else {
      session = it->second;
    }

    session->connection()->ProcessUdpPacket(
        current_server_address_, current_client_address_, *current_packet_);

#### QuicPacketWriter
To handle packet writing you will have to implement the QuicPacketWriter interface.
The important method to implement is the WritePacket method.
It takes a buffer of raw data, the length of the buffer, the own IP address and the IP address of the peer.
It is responsible for writing the data to a network socket.

    WriteResult QuicDefaultPacketWriter::WritePacket(
        const char* buffer,
        size_t buf_len,
        const IPAddressNumber& self_address,
        const IPEndPoint& peer_address) {
      WriteResult result = QuicSocketUtils::WritePacket(
          fd_, buffer, buf_len, self_address, peer_address);
      if (result.status == WRITE_STATUS_BLOCKED) {
        write_blocked_ = true;
      }
      return result;
    }

#### Epoll server
To asynchroniously handle the network connection, epoll can be used.
An implementation of an epoll server is already present in the chromium source code.

### Sub classes
For each the server and the client, you need at least to subclass the QuicSession class and likely the QuicDataStream class.

#### Session sub classes
All QuicSession sub classes have to implement:
* *GetCryptoStream*
* *CreateIncomingDataStream*
* *CreateOutgoingDataStream*

The last to methods are called when a new QuicDataStream (sub) class must be created.
By implementing these methods the session can be made to use a custom QuicDataStream sub class.

The client session class inherits from QuicClientBaseSession.
It has to additionally implement *OnProofValid* and *OnProofVerifyDetailsAvailable* as they are purely virtual in the base class.
These have be implemented if you want to have secure connections but can be no-ops otherwise.
You also have to implement the creation of the crypto stream and start the crypto negotiation.

    void QuicClientSession::InitializeSession(
        const QuicServerId& server_id,
        QuicCryptoClientConfig* crypto_config) {
      crypto_stream_.reset(
          new QuicCryptoClientStream(server_id, this, nullptr, crypto_config));
      QuicClientSessionBase::InitializeSession();
    }

The server session class inherits directly from QuicSession.
Here too, you have to implement handling of the crypto stream.
Most likely you want to override CreateIncomingDataStream and make it return an instance of your own stream class.

    QuicDataStream* QuicServerSession::CreateIncomingDataStream(QuicStreamId id) {
      return new QuicServerStream(id, this);
    }

#### Stream sub classes
In the client, you can probably use the existing QuicDataStream.

In the server you want to inherit from QuicDataStream.
On creation of your stream class you **have to** call sequencer()->FlushBufferedFrames() to unblock the sequencer.
Otherwise it will not pass new data to the stream until the headers of a SPDY request have been received.
You can then override ProcessRawData which will receive all data for this stream.

## Open Questions
* Do we need the TimeWaitListManager to have a valid QUIC implementation?
* Why do we have to create the CryptoStream our self?
* Where can you set the certificates you accept?
