# CMPUT 664 Project - LLVM JIT Security Evaluation


### Make sure LLVM is build and somewhere on your path.
### To compile the Jit run the following command.
```
clang++ -g JitDriver.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native` -o Jit
```
### Example usage:
```
clang -emit-llvm -S helloworld.c
```
```
./Jit -dump-jitted-objects=false helloworld.ll
```
