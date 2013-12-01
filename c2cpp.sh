#!/bin/bash

for i in `find . -type f -name "*.c"`
do
    mv -v ${i} ${i%.c}.cpp
done
