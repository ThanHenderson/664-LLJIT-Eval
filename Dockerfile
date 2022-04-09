FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    apt-utils \
    clang \
    gcc \
    git \
    make \
    cmake \
    ninja-build \
    ccache \
    vim

COPY . /home/Eval/
WORKDIR /home/Eval/

RUN git submodule init && git submodule update

RUN make llvm

RUN make qjs

RUN . init.sh && make lljit
