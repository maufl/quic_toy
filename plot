#!/usr/bin/env gnuplot
set terminal png size 1024,768
set xlabel "Time (s)"
set ylabel "Bytes transfered"
set key bmargin horizontal

do for [delay in "12 50 250"] {
do for [bw in "1 5 10 100 1000"] {

set title sprintf("QUIC vs. TCP (%dms RTT, %s Mb/s)", (delay+0)*2, bw)
set output sprintf("quic_vs_tcp_%dms_%sMbs.png", (delay+0)*2, bw)
plot sprintf("server_._quic_perf_server_delay=%s_loss=0_bw=%s", delay, bw) with points pointtype 8 title "QUIC (0% loss)", \
     sprintf("server_._quic_perf_server_delay=%s_loss=1_bw=%s", delay, bw) with points pointtype 10 title "QUIC (1% loss)", \
     sprintf("server_._quic_perf_server_delay=%s_loss=5_bw=%s", delay, bw) with points pointtype 23 title "QUIC (5% loss)", \
     sprintf("server_._tcp_perf_server_delay=%s_loss=0_bw=%s", delay, bw) with points pointtype 2 title "TCP (0% loss)", \
     sprintf("server_._tcp_perf_server_delay=%s_loss=1_bw=%s", delay, bw) with points pointtype 17 title "TCP (1% loss)", \
     sprintf("server_._tcp_perf_server_delay=%s_loss=5_bw=%s", delay, bw) with points pointtype 32 title "TCP (5% loss)"
}
}