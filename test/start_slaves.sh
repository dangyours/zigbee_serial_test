#!/bin/bash

# 检查参数是否存在
if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Usage: $0 <starting_number> <device_offset>"
    echo "  <starting_number>: 起始编号"
    echo "  <device_offset>: 从第几个ttyUSB开始(从0开始计数)"
    exit 1
fi

start_num=$1
device_offset=$2

# 获取所有ttyUSB设备并按数字排序
devices=($(ls /dev/ttyUSB* | sort -V))

# 检查是否有足够的ttyUSB设备
if [ ${#devices[@]} -eq 0 ]; then
    echo "No ttyUSB devices found!"
    exit 1
fi

if [ $device_offset -ge ${#devices[@]} ]; then
    echo "Error: device_offset $device_offset is out of range (max ${#devices[@]})"
    exit 1
fi

# 计算实际可用的设备数量
available_devices=${#devices[@]}
usable_devices=$((available_devices - device_offset))

echo "Found $available_devices ttyUSB devices:"
printf '%s\n' "${devices[@]}"
echo "Using ${devices[$device_offset]} and following $((usable_devices - 1)) devices"

# 启动slave进程
for ((i=0; i<$usable_devices; i++))
do
    device_index=$((device_offset + i))
    current_num=$((start_num + i))
    
    if ((current_num < 10)); then
        num_str="0$current_num"
    else
        num_str="$current_num"
    fi
    
    echo "Starting slave on ${devices[$device_index]} with number $num_str"
    ../serial_slave_all "${devices[$device_index]}" "$num_str" &
done

echo "Started $usable_devices slaves with numbers from $start_num"


