#!/bin/bash
chmod u+x GlobalServer/GlobalServer
chmod u+x GameServer/GameServer
chmod u+x GateServer/GateServer
chmod u+x WebServer/WebServer
#chmod u+x PubServer/PubServer

ps -ef | grep `pwd` | grep -v grep | grep -v Up | grep -v Login |  cut -c10-14 | xargs -I {} kill {}

curDir=$(pwd)
sleep 1
$curDir/GlobalServer/GlobalServer -d -w 1
sleep 1
$curDir/GameServer/GameServer -d -w 2
sleep 1
$curDir/WebServer/WebServer -d -w 3
sleep 2
$curDir/GateServer/GateServer -d -w 4
#sleep 2
#$curDir/PubServer/PubServer -d -w 5
