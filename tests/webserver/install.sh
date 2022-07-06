#!/bin/bash
#

STATUS=$(dpkg -l |grep lighttpd |cut -d " " -f 1)
if [ $STATUS != "ii" ]; then
    sudo apt install lighttpd
fi

source ./common.inc

mkdir -p $ROOT_DIR/logs $ROOT_DIR/pages

cp ./public/* $ROOT_DIR/pages/
