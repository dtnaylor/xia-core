#!/bin/bash
sudo echo testing
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
firefox &
cd ../
sudo ./click-2.0/userlevel/click ./click-2.0/conf/xia_ipv4.click &
cd ./proxies
sudo ifconfig fake0 mtu 65521
sudo ifconfig fake1 mtu 65521
python ./webserver.py &
python ./proxy.py 10000 &