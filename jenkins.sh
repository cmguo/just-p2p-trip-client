#!/bin/sh

export BRANCH=master
export PLATFORM=linux-x64

BUILD_NUMBER=$(cat BUILD_NUMBER)
if [ -z ${BUILD_NUMBER} ]
then
    BUILD_NUMBER=1
else
    BUILD_NUMBER=$(expr ${BUILD_NUMBER} + 1)
fi
echo ${BUILD_NUMBER} > BUILD_NUMBER
export BUILD_NUMBER

~/bin/build/build-it.sh
