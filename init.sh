#!/bin/bash

export CCOMP=$(dirname $(readlink -f "$0"))/664-llvm/build/bin/clang
export CXXCOMP=$(dirname $(readlink -f "$0"))/664-llvm/build/bin/clang++
export LLVMCONFIG=$(dirname $(readlink -f "$0"))/664-llvm/build/bin/llvm-config
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:$(dirname $(readlink -f "$0"))/664-llvm/build/lib
