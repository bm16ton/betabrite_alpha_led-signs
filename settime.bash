#!/bin/bash
echo "/dev/ttyX needs chmod aou+wrx or something similiar to work sudo wont pass correctly"
 printf '\001'"Z""00"'\002'"E"" "$1'\004' | tee -a /dev/ttyUSB0
