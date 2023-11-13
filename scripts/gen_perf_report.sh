# 定义变量pname，赋值为 "bench"
pname=bench

# 使用 pgrep 命令根据进程名 "pname" 查找进程ID，并将结果赋值给变量 pid
# -f means use full process name to match

pid=`pgrep -f ${pname}`

# 定义变量 seconds，赋值为 10
seconds=5

# 定义变量 name，赋值为 "out"
name=out

# 检查是否定义了最大检测时长，如果用户在运行脚本时提供了参数，则将参数值赋给 seconds 变量
if [ "$1" ];
then
	seconds=$1
fi

# 检查是否定义了输出文件名，如果用户在运行脚本时提供了第二个参数，则将参数值赋给 name 变量
if [ "$2" ];
then
	name=$2
fi

# 如果找到了进程ID，执行以下代码块
if [[ $pid != "" ]]
then
	# 输出提示信息，表示要对 pname 进程进行性能记录，持续 seconds 秒
	echo "GOING TO RECORD ${pname} FOR ${seconds} SECOND"
else
	# 输出提示信息，表示没有找到 "pname" 进程，无法进行性能记录
	echo "PLEASE DEPLOY pname"
	# 退出脚本并返回状态码 1，表示出现错误
	exit 1
fi

echo ${pname} pid is $pid

# 使用 perf 工具以指定的参数记录 cache_server 进程的性能数据
# -F 99: 指定采样频率为 99 赫兹
# -p ${pid}: 指定要记录的进程ID，即 cache_server 进程的PID
# -g: 启用调用图记录，记录函数调用的层次结构
# -- sleep ${seconds}: 通过 sleep 命令使 perf 记录执行指定时长，${seconds} 是之前定义的时长，默认为 10 秒

sudo perf record -F 99  -p ${pid} -g -- sleep ${seconds}
#sudo perf record -F 99  -p ${pid} --call-graph dwarf -- sleep ${seconds}
#sudo perf record -F 99  -p ${pid} --call-graph lbr -- sleep ${seconds}

# 生成可读的报告，将 perf.data 中的性能数据输出到 perf.txt 文件中
sudo perf report -i perf.data > perf.txt

# 准备生成火焰图，将 perf.data 转换为可读的格式，存储在 ${name}.perf 文件中
sudo perf script > ${name}.perf


FlameGraph=/home/zhuziyi/utils/FlameGraph
# 使用 FlameGraph 工具将 ${name}.perf 文件转换为可视化的火焰图
# 首先，使用 stackcollapse-perf.pl 将 ${name}.perf 文件折叠成 folded 格式
# 然后，使用 flamegraph.pl 将 folded 格式转换为 SVG 格式的火焰图，并将结果存储在 ${name}.svg 文件中
sudo ${FlameGraph}/stackcollapse-perf.pl ${name}.perf > ${name}.folded
sudo ${FlameGraph}/flamegraph.pl ${name}.folded > ${name}.svg
