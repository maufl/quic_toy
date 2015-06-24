# QUIC toy
To evaluate QUIC and to get familiar with the implementation in Chromium, we wrote a "simple" QUIC client and server.
This repository contains this client, server as well as a TCP client and server for comparison.
The QUIC client and server depend on [libquic](https://github.com/devsisters/libquic), which is the Chromium QUIC implementation extracted into a library.
Furthermore, there is a Python script that executes test cases to compare these to protocols and a Gnuplot script do graph the results.

The [documentation](documentation.markdown) file contains our findings and understanding of the QUIC implementation.
The [howto](howto.markdown) file contains a description on how to implement your own QUIC using application, using the libquic library.
The [performance](performance.markdown) file contains the results of our performance test and instructions on how to reproduce them.
