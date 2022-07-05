#!/bin/bash
#
# Based on info from: https://blog.devolutions.net/2020/07/tutorial-how-to-generate-secure-self-signed-server-and-client-certificates-with-openssl/
#

source ./common.inc
RNDPASS=__SecureServerPassw00rd__

if [ ! -f /usr/bin/openssl ]; then
    sudo apt install openssl
fi

mkdir -p $ROOT_DIR/ca $ROOT_DIR/private $ROOT_DIR/certs

# Step 1 - Certificate Authority
## Step 1.1 - Generate the Certificate Authority (CA) Private Key
openssl ecparam -name prime256v1 -genkey -noout -out $ROOT_DIR/ca/ca.key

## Step 1.2 - Generate the Certificate Authority Certificate
openssl req -new -x509 -sha256 -key $ROOT_DIR/ca/ca.key -out $ROOT_DIR/ca/ca.crt -subj "/C=DK/ST=Denial/L=Odense/O=Dis/CN=ca.localhost"

cat $ROOT_DIR/ca/ca.key > $ROOT_DIR/ca/ca.pem
cat $ROOT_DIR/ca/ca.crt >> $ROOT_DIR/ca/ca.pem

# Step 2: Server Certificate
## Step 2.1 - Generate the Server Certificate Private Key
openssl ecparam -name prime256v1 -genkey -noout -out $ROOT_DIR/private/server.key

## Step 2.2 - Generate the Server Certificate Signing Request
openssl req -new -sha256 -key $ROOT_DIR/private/server.key -out server.csr -subj "/C=DK/ST=Denial/L=Odense/O=Dis/CN=server.localhost"

## Step 2.3 - Generate the Server Certificate
openssl x509 -req -in server.csr -CA $ROOT_DIR/ca/ca.crt -CAkey $ROOT_DIR/ca/ca.key -CAcreateserial -out $ROOT_DIR/certs/server.crt -days 1000 -sha256

rm server.csr

cat $ROOT_DIR/private/server.key > $ROOT_DIR/certs/server.pem
cat $ROOT_DIR/certs/server.crt >> $ROOT_DIR/certs/server.pem
