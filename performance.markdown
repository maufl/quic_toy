# Performance evaluation
To determine whether we can use QUIC, we did some simple performance tests.
We used [Mininet](http://mininet.org/) to construct a virtual network in which we can test QUIC and compare it to TCP.
The network simulates only two hosts, connected by a single link.

```
+----+      -----+
|    |  L1  |    |
| H1 <------> H2 |
|    |      |    |
+----+      +----+
```

On host H1 we started the QUIC (or TCP) server and on H2 we started the QUIC (or TCP) client.
The client then sends as much data as possible for 30 seconds.
The server writes the byte count of the received data to stdout each second.

We ran the test for several different values of parameters of the link.
These parameters include the latency of the link, the bandwidth and the packet loss rate.

# Steps to reproduce
Unfortunately, Mininet is only available on Linux, so you need a Linux host with Mininet installed.

1. Compile the QUIC/TCP client and server.

    You need to have [libquic](https://github.com/devsisters/libquic) checked out in the same folder than this project and successfully build.
    Then you should be able to run `make` to build everything.

2. Run `performance_test.py` as root! (Mininet requires to be run as root.)

    The performance_test.py will iterate over several combinations of latency, bandwidth and loss rate.
    By changing a function call, there is the possibility to either execute all test in sequence or some in parallel.
    Take a look at the script to see how to increase the number of tests, executed in parallel.
    Beware that running the test ins sequence can take a long time! Each test run takes at least 30 seconds

3. Run `plot` to plot the graphs. (Needs gnuplot installed.)

# Results
In general it seems that QUIC is able to transport more data at a higher packet loss rates, while TCP does perform better at low packet loss rates and high bandwidth.
It is unclear however what influence the software of the virtual network has on the protocol performance.
Furthermore, the default settings for send and receive windows where used. QUIC allows to customize several parameters of the flow control and we have not tested whether changing those would improve throughput.

Our measurements and graphs can be found in the `results` folder.
At all bandwidths, TCP outperforms QUIC on low packet loss rates and this becomes more visible at higher bandwidths.

To improve the accuracy of our results, we ran the test for all parameters multiple times and then aggregated (after sorting out failed test runs) all collected datarates for each parameter combination.
The mean datarates for each parameter combination can be seen in the following graphs.
![Graph: 5 Mbps](results/datarate-to-loss_5Mbps.png)
![Graph: 10 Mbps](results/datarate-to-loss_10Mbps.png)
At 100 MBps the performance difference between TCP and QUIC becomes visible.
![Graph: 100 Mbps](results/datarate-to-loss_100Mbps.png)
