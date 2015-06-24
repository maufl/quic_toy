#!/usr/bin/env python2

import time
import re
from multiprocessing import Pool
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import RemoteController
from mininet.cli import CLI
from mininet.log import setLogLevel
from mininet.link import TCLink

#setLogLevel("info")

POOL_SIZE = 8
RUN_TIME = 30

class SimpleTopology(Topo):

    def build(self, **opts):
        h1 = self.addHost('h1', ip="192.168.0.1/24", mac="cc:cc:cc:cc:cc:01")
        h2 = self.addHost('h2', ip="192.168.0.2/24", mac="cc:cc:cc:cc:cc:02")
        self.addLink(h1, h2, **opts)

    @staticmethod
    def create_net(**opts):
        return Mininet(topo=SimpleTopology(**opts), link=TCLink)

def performance_test(srv_cmd, clt_cmd, **opts):
    print("Starting performance test with parameters %s" % (opts))
    net = SimpleTopology.create_net(**opts)
    net.start()
    h1 = net.get('h1')
    h2 = net.get('h2')
    ofile = open('server_%s_%s' % (re.sub('[ /]', '_', srv_cmd), '_'.join("%s=%r" % (key,val) for (key,val) in opts.iteritems())), 'w')
    print("Running server:\n%s" % srv_cmd)
    h1.sendCmd(srv_cmd + " 2>>error_log")
    time.sleep(1)
    h2.cmd(clt_cmd)
    result = h1.waitOutput()
    ofile.write(result)
    ofile.close()
    net.stop()
    print("Performance test finished")

def run_all_tests_parallel():
    pool = Pool(POOL_SIZE)
    for delay in (12, 50, 250):
        for bw in (1, 5, 10, 100, 1000):
            for loss in (0, 1, 5):
                r = pool.apply_async(performance_test,
                                     ['./quic_perf_server',
                                      "./quic_perf_client -d=%s 192.168.0.1" % RUN_TIME],
                                      dict(delay=delay, bw=bw, loss=loss)
                                     )
                r = pool.apply_async(performance_test,
                                     ['./tcp_perf_server',
                                      "./tcp_perf_client -d=%s 192.168.0.1" % RUN_TIME],
                                      dict(delay=delay, bw=bw, loss=loss)
                                     )
    pool.close()
    pool.join()

def run_all_tests_sequentially():
    for delay in (12, 50, 250):
        for bw in (1, 5, 10, 100, 1000):
            for loss in (0, 1, 5):
                performance_test('./quic_perf_server', "./quic_perf_client -d=%s 192.168.0.1" % RUN_TIME, delay=delay, bw=bw, loss=loss)
                performance_test('./tcp_perf_server', "./tcp_perf_client -d=%s 192.168.0.1" % RUN_TIME,  delay=delay, bw=bw, loss=loss)

def run_cli():
    net = SimpleTopology.create_net()
    net.start()
    CLI(net)
    net.stop()

run_all_tests_parallel()
