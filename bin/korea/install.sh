#! /bin/sh


# 检查第一个参数
if [ "$1" != "start" ]; then
    echo "参数不正确，退出脚本。"
    exit 1
fi

cd ..
rm -fr last_version
mkdir last_version
mv conf  GameServer  GateServer  GlobalServer PubServer tblres  WebServer last_version
echo "save last_version"


cd ../src/linuxbuild
echo "当前路径 : "`pwd`
unset DEBUG_ENV
unset ATHENA_OPTIONS
unset TX_YYB_ENV
export DEBUG_ENV ATHENA_OPTIONS TX_YYB_ENV

make clean
cmake ..
make -j8
cd ../../bin/korea

echo "复制资源到 bin 目录"
mkdir -p ../tblres && cp -af ../../rescfg/tbl/*.tbl   ../tblres
mkdir -p ../conf && cp -af ../../rescfg/xml/*.xml   ../conf
echo -e "复制资源到 bin 目录完毕"

echo -e 进入目录`pwd`
echo "复制程序到../目录"
mkdir -p ../GameServer/
# objcopy   --only-keep-debug ../../src/GameServer/GameServer  ../GameServer/GameServer.symbol
objcopy   --strip-debug ../../src/GameServer/GameServer  ../GameServer/GameServer
mkdir -p ../GateServer/
# objcopy   --only-keep-debug ../../src/GateServer/GateServer  ../GateServer/GateServer.symbol
objcopy   --strip-debug ../../src/GateServer/GateServer  ../GateServer/GateServer
mkdir -p ../GlobalServer/
# objcopy   --only-keep-debug ../../src/GlobalServer/GlobalServer  ../GlobalServer/GlobalServer.symbol
objcopy   --strip-debug ../../src/GlobalServer/GlobalServer  ../GlobalServer/GlobalServer
mkdir -p ../WebServer/
# objcopy   --only-keep-debug ../../src/WebServer/WebServer  ../WebServer/WebServer.symbol
objcopy   --strip-debug ../../src/WebServer/WebServer  ../WebServer/WebServer
mkdir -p ../PubServer/
# objcopy   --only-keep-debug ../../src/PubServer/PubServer  ../PubServer/PubServer.symbol
objcopy   --strip-debug ../../src/PubServer/PubServer  ../PubServer/PubServer
echo -e "复制程序到../目录完毕\n"

echo -e "发布完成 SUCCESS"
