#!/usr/bin/env python2

import time
import re
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import RemoteController
from mininet.cli import CLI
from mininet.log import setLogLevel
from mininet.link import TCLink

#setLogLevel("info")

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
    cmd = '%s > server_%s_%s &' % (srv_cmd, re.sub('[ /]', '_', srv_cmd),
                                    '_'.join("%s=%r" % (key,val) for (key,val) in opts.iteritems()))
    print("Running server:\n%s" % cmd)
    h1.cmd(cmd)
    h2.cmd(clt_cmd)
    time.sleep(5)
    net.stop()
    print("Performance test finished")

def run_all_tests():
    time = 60
    for loss in (0, 1, 5):
        performance_test('./quic_perf_server',"./quic_perf_client -d=%s 192.168.0.1" % time, bw=5,loss=loss)
    for loss in (0, 1, 5):
        performance_test('./tcp_perf_server',"./tcp_perf_client -d=%s 192.168.0.1" % time, bw=5,loss=loss)

def run_cli():
    net = SimpleTopology.create_net()
    net.start()
    CLI(net)
    net.stop()

run_all_tests()
