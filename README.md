# CMPUT 664 Project - LLVM JIT Security Evaluation

### Build LLVM with the following script in a separate 

```
cmake ../llvm                                         \
  -GNinja                                             \
  -DLLVM_TARGETS_TO_BUILD=X86                         \
  -DLLVM_OPTIMIZED_TABLEGEN=ON                        \
  -DLLVM_INSTALL_UTILS=ON                             \
  -DLLVM_CCACHE_BUILD=ON                              \
  -DBUILD_SHARED_LIBS=ON                              \
  -DLLVM_ENABLE_RTTI=ON                               \
  -DLLVM_PARALLEL_LINK_JOBS=8                         \
  -DCMAKE_BUILD_TYPE=Release                          \
  -DLLVM_ENABLE_PROJECTS="clang;lldb;compiler-rt"     \
  -DCMAKE_C_COMPILER=clang                            \
  -DCMAKE_CXX_COMPILER=clang++  
```

### Build LLJIT on your system.

Before running make, you must export the following environment variables.
Replace USERNAME with your linux username.
```
export CCOMP=/home/USERNAME/664-llvm/build/bin/clang
export CXXCOMP=/home/USERNAME/664-llvm/build/bin/clang++
export LLVMCONFIG=/home/USERNAME/664-llvm/build/bin/llvm-config
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/home/USERNAME/664-llvm/build/lib
```

### Running a program
```
./LLJIT PROGRAMNAME
```
