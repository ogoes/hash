#!/bin/sh





files=$(ls protos)


for file in $files; do
	protoc -I=./protos --cpp_out=./ $file
done
