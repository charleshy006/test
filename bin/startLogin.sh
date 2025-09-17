#!/bin/bash
chmod u+x LoginServer/LoginServer

ps -ef | grep `pwd` | grep Login | grep -v grep | cut -c10-14 | xargs -I {} kill {}
curDir=$(pwd)
sleep 1
$curDir/LoginServer/LoginServer -d -w 1007

