#!/usr/bin/env gnuplot
set terminal png size 1024,768
set xlabel "Time (s)"
set ylabel "Mbit transfered"
set key outside center bottom vertical maxrows 3

do for [delay in "12 50 250"] {
do for [bw in "1 5 10 100 1000"] {

set title sprintf("QUIC vs. TCP (%dms RTT, %s Mbps)", (delay+0)*2, bw)
set output sprintf("quic_vs_tcp_%dms_%sMbps.png", (delay+0)*2, bw)
plot [1:30] \
     sprintf("server_._quic_perf_server_delay=%s_loss=0_bw=%s", delay, bw) index 0:30 u ($0+1):($1 * 8/1e6) \
     with linespoints pt 5 lc rgb "#edb120" title "QUIC (0% loss)", \
     sprintf("server_._quic_perf_server_delay=%s_loss=1_bw=%s", delay, bw) index 0:30 u ($0+1):($1 * 8/1e6) \
     with linespoints pt 5 lc rgb "#d95319"title "QUIC (1% loss)", \
     sprintf("server_._quic_perf_server_delay=%s_loss=5_bw=%s", delay, bw) index 0:30 u ($0+1):($1 * 8/1e6) \
     with linespoints pt 5 lc rgb "#a2142f" title "QUIC (5% loss)", \
     sprintf("server_._tcp_perf_server_delay=%s_loss=0_bw=%s", delay, bw) index 0:30 u ($0+1):($1 * 8/1e6) \
     with linespoints pt 9 lc rgb "#4dbeee" title "TCP (0% loss)", \
     sprintf("server_._tcp_perf_server_delay=%s_loss=1_bw=%s", delay, bw) index 0:30 u ($0+1):($1 * 8/1e6) \
     with linespoints pt 9 lc rgb "#0072bd" title "TCP (1% loss)", \
     sprintf("server_._tcp_perf_server_delay=%s_loss=5_bw=%s", delay, bw) index 0:30 u ($0+1):($1 * 8/1e6) \
     with linespoints pt 9 lc rgb "#7e2f8e" title "TCP (5% loss)", \
     (bw * x) with lines lw 2 lc rgb "#77ac30" title "Target bandwidth"
}
}