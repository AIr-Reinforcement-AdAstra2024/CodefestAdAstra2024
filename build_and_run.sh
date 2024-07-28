#!/bin/bash
cd CodefestAdAstra2024/build
cmake --build .
if [ $? -ne 0 ]; then
    exit 1
fi
cd ..
./build/CodefestAdAstra2024 "$@"

