#!/bin/bash
apt-get update -y
apt-get install ssh -y
sed -i 's/#Port 22/Port 2292/g' /etc/ssh/sshd_config
/etc/init.d/ssh restart
