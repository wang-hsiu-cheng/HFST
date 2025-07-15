# Use Ubuntu as base image
FROM ubuntu:latest AS base

# Set the maintainer
LABEL maintainer="Ted Wang wang.tw.ted@gmail.com"

ARG USERNAME=ted
ARG USER_UID=1001
ARG USER_GID=1001

FROM base AS user-setup

RUN \
    groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo gosu \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    && rm -rf /var/lib/apt/lists/*

FROM user-setup AS tools

# Install Development Tools and dependencies
RUN apt update && apt install -y \
    build-essential cmake automake autoconf \
    tmux \
    htop \
    tree \
    vim \
    git \
    wget \
    curl \
    unzip \
    usbutils \    
    gcc \
    g++ \
    make \
    python3 \
    python3-pip \
    bash-completion

RUN apt-get update && apt-get dist-upgrade -y \
    && apt-get autoremove -y \
    && apt-get autoclean -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

FROM tools AS final

# Set up the user environment
ENV TZ=Asia/Taipei
ENV SHELL=/bin/bash
ENV TERM=xterm-256color

# Set up working directory
USER ted
WORKDIR /home

# COPY .bashrc /home/$USERNAME/.bashrc.conf
# RUN cat /home/$USERNAME/.bashrc.conf >> /home/$USERNAME/.bashrc

# Set the default command to bash
CMD ["/bin/bash", "-ic"]