#!/bin/bash

#if [ $# -lt 1 ] ; then
#	echo "必须输入参数"
#	exit 0
#fi

allFile=$(ls  *.proto)

for file in $allFile  ; do
	protoc  -I=. --cpp_out=../src/Proto  $file
done

