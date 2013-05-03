#!/bin/bash

# Copyright (c) 2013 Alexander Ignatyev. All rights reserved.
# The script should be run from project root directory due to path of cpplint.py.

if [ "$1" == "--help" ] || [ "$1" == "" ] || [ "$2" == "" ];
then
    echo "USAGE: tools/static_checker.sh cpplint|cppcheck src|tests"
fi

if [ "$1" == "cpplint" ];
then
    python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" `find $2` 2>&1
fi

if [ "$1" == "cppcheck" ];
then
    cppcheck --quiet --enable=all --inconclusive --std=c++11 -UNDEBUG $2 2>&1
fi