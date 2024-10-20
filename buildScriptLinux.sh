#!/bin/bash
cqtdeployer cqtdeployer -bin ./NatsGateWay -qmake /home/zas/Qt/6.5.3/gcc_64/bin/qmake
cp ./QtService/ ./DistributionKit/ -r
cp ./QtService/plugins/servicebackends/ ./DistributionKit/plugins/ -r
cp ./QtService/plugins/servicebackends/libqsystemd.so ./DistributionKit/plugins/libqsystemd.so
cp ./QtService/plugins/servicebackends/libqstandard.so ./DistributionKit/plugins/libqstandard.so
cp ./NatsGateWay.service /etc/systemd/system/NatsGateWay.service
cp ./NatsGateWay.conf /etc/dbus-1/system.d/NatsGateWay.conf
