#!/bin/bash
#
# Based on info from: https://blog.devolutions.net/2020/07/tutorial-how-to-generate-secure-self-signed-server-and-client-certificates-with-openssl/
#
ROOT_DIR=ssl

if [ ! -f /usr/bin/openssl ]; then
    sudo apt install openssl
fi

mkdir -p $ROOT_DIR/ca $ROOT_DIR/private $ROOT_DIR/certs

# Step 1 - Certificate Authority
## Step 1.1 - Generate the Certificate Authority (CA) Private Key
openssl ecparam -name prime256v1 -genkey -noout -out $ROOT_DIR/ca/ca.key

## Step 1.2 - Generate the Certificate Authority Certificate
openssl req -new -x509 -sha256 -key $ROOT_DIR/ca/ca.key -out $ROOT_DIR/ca/ca.crt -subj "/C=DK/ST=Denial/L=Odense/O=RSP/CN=ca.localhost"

cat $ROOT_DIR/ca/ca.key > $ROOT_DIR/ca/ca.pem
cat $ROOT_DIR/ca/ca.crt >> $ROOT_DIR/ca/ca.pem

# Step 2: Server Certificate
## Step 2.1 - Generate the Server Certificate Private Key
openssl ecparam -name prime256v1 -genkey -noout -out $ROOT_DIR/private/server.key

## Step 2.2 - Generate the Server Certificate Signing Request
openssl req -new -sha256 -key $ROOT_DIR/private/server.key -out server.csr -subj "/C=DK/ST=Denial/L=Odense/O=RSP/CN=server.localhost"

## Step 2.3 - Generate the Server Certificate
openssl x509 -req -in server.csr -CA $ROOT_DIR/ca/ca.crt -CAkey $ROOT_DIR/ca/ca.key -CAcreateserial -out $ROOT_DIR/certs/server.crt -days 1000 -sha256

rm server.csr

cat $ROOT_DIR/private/server.key > $ROOT_DIR/certs/server.pem
cat $ROOT_DIR/certs/server.crt >> $ROOT_DIR/certs/server.pem


# Step 3: Client Certificate
CLIENT_NAME=SN1234

## Step 3.1 - Generate the Client Certificate Private Key
openssl ecparam -name prime256v1 -genkey -noout -out $ROOT_DIR/private/$CLIENT_NAME.key

## Step 3.2 - Create the Client Certificate Signing Request
openssl req -new -sha256 -key $ROOT_DIR/private/$CLIENT_NAME.key -out $CLIENT_NAME.csr -subj "/C=DK/ST=Denial/L=Odense/O=RSP/CN=$CLIENT_NAME"

## Step 3.3 - Generate the Client Certificate
openssl x509 -req -in $CLIENT_NAME.csr -CA $ROOT_DIR/ca/ca.crt -CAkey $ROOT_DIR/ca/ca.key -CAcreateserial -out $ROOT_DIR/certs/$CLIENT_NAME.crt -days 1000 -sha256

rm $CLIENT_NAME.csr

cat $ROOT_DIR/private/$CLIENT_NAME.key $ROOT_DIR/certs/$CLIENT_NAME.crt > $ROOT_DIR/certs/$CLIENT_NAME.pem

