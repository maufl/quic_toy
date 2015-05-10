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

### Sub classes
For each the server and the client, you need at least to subclass the QuicSession class and likely the QuicDataStream class.

#### Session sub classes
All QuicSession sub classes have to implement:
* GetCryptoStream
* CreateIncomingDataStream
* CreateOutgoingDataStream
The last to methods are called when a new QuicDataStream (sub) class must be created.
By implementing these methods the session can be made to use a custom QuicDataStream sub class.

The client session class inherits from QuicClientBaseSession.
It has to additionally implement OnProofValid and OnProofVerifyDetailsAvailable as they are purely virtual in the base class.
These have be implemented if you want to have secure connections but can be no-ops otherwise.
You also have to implement the creation of the crypto stream and start the crypto negotiation.

The server session class inherits directly from QuicSession.
Here too, you have to implement handling of the crypto stream.
Most likely you want to override CreateIncomingDataStream and make it return an instance of your own stream class.

#### Stream sub classes
In the client, you can probably use the existing QuicDataStream.

In the server you want to inherit from QuicDataStream.
On creation of your stream class you **have to** call sequencer()->FlushBufferedFrames() to unblock the sequencer.
Otherwise it will not pass new data to the stream until the headers of a SPDY request have been received.
You can then override ProcessRawData which will receive all data for this stream.

### New classes
You will need additional classes to have a functional QUIC application.

#### QuicPacketWriter
To handle packet writing you will have to implement the QuicPacketWriter interface.
The important method to implement is the WritePacket method.
It takes a buffer of raw data, the length of the buffer, the own IP address and the IP address of the peer.
It is responsible for writing the data to a network socket.

#### Epoll server
To asynchroniously handle the network connection, epoll can be used.
An implementation of an epoll server is already present in the chromium source code.

#### QUIC client application
You also need a class that will read the network packages, dispatches it to the session, creates a session, connection, packet writer and connects all.
It will usually create a new session and connection, passing the connection to the session.
It must also create a QuicPacketWriter(Factory) and pass it to the connection, which will likely require creating a UDP socket.
If you want to have async network communication and want to use the epoll server it also has to implement the EpollCallbackInterface.
It then has to read the network packages when it is notified by the epoll server and call the ProcessUdpPacket method of the connection.

#### QUIC server application
The server side is a bit more involved, as it has to handle multiple sessions with multiple connections.
Therefore, the server class usually uses a dispatcher class.
It will handle the reading of network packets similar to the client (using epoll).
Instead of handing it to the connection directly it will hand it to the dispatcher.

The dispatcher uses a framer to parse the packets which in turn will call the dispatcher on reading frames.
When the dispatcher has determined the connection to which the packet belongs it will call the ProcessUdpPacket of the sessions connection.
If there is no such session, the dispatcher will first create it.
