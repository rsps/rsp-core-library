ARG BASE_IMAGE=debian:13
FROM ${BASE_IMAGE}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        clang \
        clang-tools \
        libc++-dev \
        libc++abi-dev \
        cmake \
        ninja-build \
        git \
        ca-certificates \
        pkg-config \
        libssl-dev \
        libcurl4-openssl-dev \
        libwpa-client-dev \
        libfreetype-dev \
        zlib1g-dev \
        libsdl2-dev \
        libpcre2-dev \
        jq \
        psmisc \
        lighttpd \
        lighttpd-mod-openssl \
    && rm -rf /var/lib/apt/lists/*
