#!/bin/bash

tmp=$IFS
IFS='
'
stopServr()
{
	for var  in $(ps -u $(basename $HOME) | grep $1) 
	do
		pid=$(echo $var | cut -c1-5)
		pname=$(echo $var | cut -c25-)	

		if  kill -9 $pid
		then
			echo "$pname 停止"
		else
			echo "$pname 不能够被停止"
		fi
	done
}

stopServr "GateServer"
echo "正在等待网关的关闭,需要等待20秒......"
sleep 1
stopServr "Server"

cond=$(ps -u $(basename $HOME) | grep Server | wc -l)
while [ $cond -gt 0 ]
do
	sleep 1
	cond=$(ps -u $(basename $HOME) | grep Server | wc -l)
	echo "仍然没有停止的ServerNum:$cond"
done
