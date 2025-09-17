#!/bin/bash

# 源文件夹
SOURCE_DIR="/data/workspace_H/Server/bin"
SERVERS=("GlobalServer" "GameServer" "GateServer" "LoginServer" "PubServer" "WebServer" "conf" "tblres")

# 目标文件夹数量（例如: 10 表示 B1, B2, B3）
NUM_TARGETS=10

# 确保源文件夹存在
if [ ! -d "$SOURCE_DIR" ]; then
    echo "源文件夹 $SOURCE_DIR 不存在!"
    exit 1
fi

# 遍历每个目标文件夹
for (( i=1; i<=NUM_TARGETS; i++ )); do
    TARGET_DIR="/data/gameservers/game${i}/server/bin"


    # 检查目标文件夹是否存在
    if [ ! -d "$TARGET_DIR" ]; then
        echo "目标文件夹 $TARGET_DIR 不存在!"
        continue
    fi

    # 复制每个源文件夹到目标文件夹
    for SERVER in "${SERVERS[@]}"; do
        if [ -d "$SOURCE_DIR/$SERVER" ]; then
            cp -r "$SOURCE_DIR/$SERVER" "$TARGET_DIR"
            echo "已将 $SOURCE_DIR/$SERVER 复制到 $TARGET_DIR"
        else
            echo "源文件夹 $SOURCE_DIR/$SERVER 不存在!"
        fi
    done

    # 复制所有的 .sh 文件
    for SH_FILE in "$SOURCE_DIR"/*.sh; do
        if [ -f "$SH_FILE" ]; then
            cp "$SH_FILE" "$TARGET_DIR"
            echo "已将 $SH_FILE 复制到 $TARGET_DIR"
        fi
    done
done

echo "复制操作完成!"
