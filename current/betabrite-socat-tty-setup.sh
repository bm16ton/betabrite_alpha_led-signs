#!/bin/bash
sudo chmod aou+wrx /dev/tty63
sudo socat PTY,link=/dev/tty63 TCP:192.168.0.222:10000
