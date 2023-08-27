# CPP-WTFSC
Write The Friendly Source Code In CPP！

# RUN
```shell
cd CPP-WTFSC

# 扫描当前目录下的 Dockerfile 构建 my_env 镜像 image
docker build -t my_env .

# 将镜像放入容器中（docker create）,然后将容器启动，使之变成运行时容器（docker start）
# -p 127.0.0.1:8080:8080：将容器的 8080 端口映射到主机的 8080 端口，使得容器内的服务可以通过 http://127.0.0.1:8080 访问
# -v ${PWD}:~/CPP-WTFSC 将当前主机目录（${PWD}）与容器内的 ~/CPP-WTFSC 目录进行挂载
# -w /home/zhuziyi/CPP-WTFSC 容器会在指定的工作目录中启动，并且后续执行的命令会在这个工作目录下执行
# --name ubuntu 将容器的名称设置为 "ubuntu"
# my_env 运行的 Docker 镜像名称
sudo docker run -it \
    -p 127.0.0.1:8080:8080 \
    -v ${PWD}:/home/zhuziyi/CPP-WTFSC \
    -w /home/zhuziyi/CPP-WTFSC \
    --name ubuntu \
    my_env 
```