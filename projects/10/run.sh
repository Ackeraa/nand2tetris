#!/bin/bash

make clean && make

# check if the file is a directory
if [ -d $1 ]; then
  # if it is, then run the test on all the files in the directory
  for file in $1/*.jack; do
    ./JackCompiler $file
    output=${file%.jack}.xml
    diff -w <(cat $output) <(cat "$output"_)
  done
else
    # if it is not, compare the output of the compiler with the expected output, ignoring spaces
    ./JackCompiler $1
    output=${1%.jack}.xml
    diff -w <(cat $output) <(cat "$output"_)
fi
