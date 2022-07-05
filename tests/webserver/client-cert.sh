#!/bin/bash
#

CLIENT_NAME=$2

source ./common.inc

# Step 3: Client Certificate
## Step 3.1 - Generate the Client Certificate Private Key
openssl ecparam -name prime256v1 -genkey pass:$CLNTPASS -noout -out $ROOT_DIR/private/$CLIENT_SERIAL.key

## Step 3.2 - Create the Client Certificate Signing Request
openssl req -new -sha256 -key $ROOT_DIR/private/$CLIENT_SERIAL.key pass:$CLNTPASS -out $CLIENT_SERIAL.csr -subj "/C=DK/ST=Denial/L=Odense/O=Dis/CN=$CLIENT_NAME"

## Step 3.3 - Generate the Client Certificate
openssl x509 -req -in $CLIENT_SERIAL.csr -CA $ROOT_DIR/ca/ca.crt -CAkey $ROOT_DIR/ca/ca.key pass:$SRVPASS -CAcreateserial -out $ROOT_DIR/certs/$CLIENT_SERIAL.crt -days 1000 -sha256

rm $CLIENT_SERIAL.csr
