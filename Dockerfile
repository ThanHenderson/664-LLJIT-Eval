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
    ccache

COPY . /home/Eval/
WORKDIR /home/Eval/

RUN git submodule init && git submodule update

RUN make llvm

RUN make qjs

RUN source init.sh

RUN make lljit
