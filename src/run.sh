#!/bin/bash

SRC_DIR=$(dirname "$0")
CERTS_DIR=$SRC_DIR/../certs
source $SRC_DIR/../.env


function CheckEnv() {
    if [ "$1" == "" ] ; then
        echo no $2 found in in .env
        exit 1
    fi
}

CheckEnv "$ENDPOINT" endpoint
CheckEnv "$CERT_FILE" "cert file path"
CheckEnv "$PRIVATE_KEY_FILE" "private key file path"
CheckEnv "$CLIENT_ID" "client id"

APP_DIR_PUB_SUB=basic_pub_sub
APP_DIR_IOT=iotapp

target=$APP_DIR_IOT
while getopts t: flag
do
    case "${flag}" in
        t) target=${OPTARG};;
    esac
done
echo "Target: $target";

if [ $target == $APP_DIR_PUB_SUB ] ; then
    LD_LIBRARY_PATH=$SRC_DIR/../artifact/lib \
    $SRC_DIR/basic_pub_sub_build/basic-pub-sub \
    --endpoint $ENDPOINT \
    --ca_file $CA_FILE \
    --cert $CERT_FILE \
    --key $PRIVATE_KEY_FILE \
    --topic $TOPIC \
    --client_id $CLIENT_ID
fi
if [ $target == $APP_DIR_IOT ] ; then
    LD_LIBRARY_PATH=./../artifact/lib iotapp_build/iotapp \
    "$ENDPOINT" \
    "$CA_FILE" \
    "$CERT_FILE" \
    "$PRIVATE_KEY_FILE" \
    "$TOPIC" \
    "$CLIENT_ID"
fi


