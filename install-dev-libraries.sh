#!/bin/bash

set -o errexit  # abort on nonzero exit status
set -o nounset  # abort on unbound variable
set -o pipefail # don't hide errors within pipes

# WARNING: This script SHOULD be executed with "sudo" privileges

# -------------------------------------------------------------------------------------------------------------- #
# Runtime dependencies
# -------------------------------------------------------------------------------------------------------------- #

apt-get install -y libssl-dev \
        libcurl4 \
        libcurl4-openssl-dev \
        libwpa-client-dev \
        libfreetype-dev \
        zlib1g-dev \
        libsdl2-dev

# -------------------------------------------------------------------------------------------------------------- #
# Runtime dependencies for tests
# -------------------------------------------------------------------------------------------------------------- #

apt-get install -y libpcre2-dev \
        pkg-config \
        jsonlint
