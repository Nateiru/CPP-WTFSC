# FROM ubuntu:latest
FROM ubuntu:22.04

# 不显示任何交互式的界面，以避免在构建过程中阻塞或中断。
ARG DEBIAN_FRONTEND=noninteractive

# apt 换源
RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list
RUN apt clean -y

# 设置系统时区为北京时区
RUN apt update \
    && apt install -y tzdata \
    && ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

# Create a no-passowrd sudo user: zhuziyi
RUN apt update \
    && apt -y install sudo \
    && useradd -m zhuziyi -s /bin/bash \
    && adduser zhuziyi sudo \
    && echo "zhuziyi ALL=(ALL) NOPASSWD : ALL" | tee /etc/sudoers.d/zhuziyi

USER zhuziyi
WORKDIR /home/zhuziyi

# Install Ubuntu packages.
# apt-get -y 自动回答安装时的确认提示

# vim 相关
RUN sudo apt -y update && \
    sudo apt -y install vim

# perf 相关工具
RUN sudo apt -y update && \
    sudo apt -y install \
    linux-tools-common \
    linux-tools-generic \
    valgrind

# 基本编译工具 from bustub
# 安装 g++12 --version
RUN sudo apt -y update && \
    sudo apt -y install \
      build-essential \
      clang-12 \
      cmake \
      git \
      g++-12

# Install zsh
RUN sudo apt install -y git zsh \
    && git clone https://github.com/ohmyzsh/ohmyzsh.git ~/.oh-my-zsh \
    && cp ~/.oh-my-zsh/templates/zshrc.zsh-template ~/.zshrc \
    && sudo usermod -s /bin/zsh zhuziyi

# 清理 apt 缓存和临时文件，以减小生成的 Docker 镜像的大小
RUN sudo apt autoremove -y \
    && sudo apt clean -y \
    && sudo rm -rf /var/lib/apt/lists/*

CMD ["zsh"]

# docker build -t my_env .
# sudo docker run -it -p 127.0.0.1:8080:8080  my_env
