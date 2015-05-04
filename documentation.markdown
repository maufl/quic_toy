# QUIC Documentation

## Classes

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
