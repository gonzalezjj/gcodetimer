#!/bin/bash

CURR_DIR=`pwd`

if [ ! -d bin ]; then
    mkdir bin
fi

cd bin

cmake ../src/
make

cd "$CURR_DIR"
