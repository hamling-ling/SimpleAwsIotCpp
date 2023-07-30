#!/bin/bash

APP_DIR_PUB_SUB=basic_pub_sub
APP_DIR_IOT=iotapp

while getopts t:a: flag
do
    case "${flag}" in
        t) target=${OPTARG};;
        a) action=${OPTARG};;
    esac
done
echo "Target: $target";
echo "Action: $action";

build() {
    APP_DIR=$1
    BUILD_DIR=${APP_DIR}_build

    rm -rf $BUILD_DIR
    mkdir $BUILD_DIR

    pushd $BUILD_DIR
    cmake ../$APP_DIR && make VERBOSE=1
    popd
}

main() {
    if [ $target == "all" ] || [ $target == $APP_DIR_PUB_SUB ] ; then
        echo building $APP_DIR_PUB_SUB
        build $APP_DIR_PUB_SUB
    fi
    if [ $target == "all" ] || [ $target == $APP_DIR_IOT ] ; then
        echo building $APP_DIR_IOT
        build $APP_DIR_IOT
    fi
}

main
