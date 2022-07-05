#!/bin/bash
#
sudo apt install lighttpd

source ./common.inc

mkdir -p $ROOT_DIR/logs $ROOT_DIR/pages

echo "<head><title>Lighty</title></head><body>Web Server</body>" > $ROOT_DIR/pages/index.html
