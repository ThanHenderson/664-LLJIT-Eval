#!/bin/bash

export CCOMP=./664-llvm/build/bin/clang
export CXXCOMP=./664-llvm/build/bin/clang++
export LLVMCONFIG=./664-llvm/build/bin/llvm-config
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:./664-llvm/build/lib
