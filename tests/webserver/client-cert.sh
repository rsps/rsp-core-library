#!/bin/bash
#

CLIENT_NAME=$1

source ./common.inc

# Step 3: Client Certificate
## Step 3.1 - Generate the Client Certificate Private Key
openssl ecparam -name prime256v1 -genkey -noout -out $ROOT_DIR/private/$CLIENT_NAME.key

## Step 3.2 - Create the Client Certificate Signing Request
openssl req -new -sha256 -key $ROOT_DIR/private/$CLIENT_NAME.key -out $CLIENT_NAME.csr -subj "/C=DK/ST=Denial/L=Odense/O=Dis/CN=$CLIENT_NAME"

## Step 3.3 - Generate the Client Certificate
openssl x509 -req -in $CLIENT_NAME.csr -CA $ROOT_DIR/ca/ca.crt -CAkey $ROOT_DIR/ca/ca.key -CAcreateserial -out $ROOT_DIR/certs/$CLIENT_NAME.crt -days 1000 -sha256

rm $CLIENT_NAME.csr

cat $ROOT_DIR/private/$CLIENT_NAME.key $ROOT_DIR/certs/$CLIENT_NAME.crt > $ROOT_DIR/certs/$CLIENT_NAME.pem
