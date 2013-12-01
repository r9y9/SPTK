#!/bin/bash

for i in `find . -type f -name "*.cpp"`
do
    mv -v ${i} ${i%.cpp}.c
done
