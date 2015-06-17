#!/usr/bin/env gnuplot
set terminal png size 1024,768
set xlabel "Time (s)"
set ylabel "Bytes transfered"
set key bmargin horizontal

set title "QUIC vs. TCP (24ms RTT, 10 Mb/s)"
set output "quic_vs_tcp_10Mbs.png"
plot "server_._quic_perf_server_delay=12_loss=0_bw=10" with points pointtype 8 title "QUIC (0% loss)", \
     "server_._quic_perf_server_delay=12_loss=1_bw=10" with points pointtype 10 title "QUIC (1% loss)", \
     "server_._quic_perf_server_delay=12_loss=5_bw=10" with points pointtype 23 title "QUIC (5% loss)", \
     "server_._tcp_perf_server_delay=12_loss=0_bw=10" with points pointtype 2 title "TCP (0% loss)", \
     "server_._tcp_perf_server_delay=12_loss=1_bw=10" with points pointtype 17 title "TCP (1% loss)", \
     "server_._tcp_perf_server_delay=12_loss=5_bw=10" with points pointtype 32 title "TCP (5% loss)"

set title "QUIC vs. TCP (24ms RTT, 100 Mb/s)"
set output "quic_vs_tcp_100Mbs.png"
plot "server_._quic_perf_server_delay=12_loss=0_bw=100" with points pointtype 8 title "QUIC (0% loss)", \
     "server_._quic_perf_server_delay=12_loss=1_bw=100" with points pointtype 10 title "QUIC (1% loss)", \
     "server_._quic_perf_server_delay=12_loss=5_bw=100" with points pointtype 23 title "QUIC (5% loss)", \
     "server_._tcp_perf_server_delay=12_loss=0_bw=100" with points pointtype 2 title "TCP (0% loss)", \
     "server_._tcp_perf_server_delay=12_loss=1_bw=100" with points pointtype 17 title "TCP (1% loss)", \
     "server_._tcp_perf_server_delay=12_loss=5_bw=100" with points pointtype 32 title "TCP (5% loss)"

set title "QUIC vs. TCP (24ms RTT, 1 Gb/s)"
set output "quic_vs_tcp_1000Mbs.png"
plot "server_._quic_perf_server_delay=12_loss=0_bw=100" with points pointtype 8 title "QUIC (0% loss)", \
     "server_._quic_perf_server_delay=12_loss=1_bw=100" with points pointtype 10 title "QUIC (1% loss)", \
     "server_._quic_perf_server_delay=12_loss=5_bw=100" with points pointtype 23 title "QUIC (5% loss)", \
     "server_._tcp_perf_server_delay=12_loss=0_bw=100" with points pointtype 2 title "TCP (0% loss)", \
     "server_._tcp_perf_server_delay=12_loss=1_bw=100" with points pointtype 17 title "TCP (1% loss)", \
     "server_._tcp_perf_server_delay=12_loss=5_bw=100" with points pointtype 32 title "TCP (5% loss)"