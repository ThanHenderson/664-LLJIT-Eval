LLVM_DIR=$(shell pwd)/664-llvm
.DEFAULT_GOAL := lljit

all : llvm quickjs lljit

llvm :
	mkdir -p $(LLVM_DIR)/build
	cd $(LLVM_DIR)/build \
	&& cmake ../llvm \
		-GNinja                                            \
		-DLLVM_TARGETS_TO_BUILD=X86                        \
		-DLLVM_OPTIMIZED_TABLEGEN=ON                       \
		-DLLVM_INSTALL_UTILS=ON                            \
		-DLLVM_CCACHE_BUILD=ON                             \
		-DBUILD_SHARED_LIBS=ON                             \
		-DLLVM_ENABLE_RTTI=ON                              \
		-DLLVM_PARALLEL_LINK_JOBS=8                        \
		-DCMAKE_BUILD_TYPE=Release                         \
		-DLLVM_ENABLE_PROJECTS="clang;lldb;compiler-rt"    \
		-DCMAKE_C_COMPILER=clang                           \
		-DCMAKE_CXX_COMPILER=clang++                       \
	&& cmake --build .

llvm-debug :
	mkdir -p $(LLVM_DIR)/build
	cd $(LLVM_DIR)/build \
	&& cmake ../llvm \
		-GNinja                                            \
		-DLLVM_TARGETS_TO_BUILD=X86                        \
		-DLLVM_OPTIMIZED_TABLEGEN=ON                       \
		-DLLVM_INSTALL_UTILS=ON                            \
		-DLLVM_CCACHE_BUILD=ON                             \
		-DBUILD_SHARED_LIBS=ON                             \
		-DLLVM_ENABLE_RTTI=ON                              \
		-DLLVM_PARALLEL_LINK_JOBS=8                        \
		-DCMAKE_BUILD_TYPE=Debug                           \
		-DLLVM_ENABLE_PROJECTS="clang;lldb;compiler-rt"    \
		-DCMAKE_C_COMPILER=clang                           \
		-DCMAKE_CXX_COMPILER=clang++                       \
	&& cmake --build .

llvm-cfi :
	mkdir -p $(LLVM_DIR)/build
	cd $(LLVM_DIR)/build \
	&& cmake ../llvm \
		-GNinja                                            \
		-DLLVM_TARGETS_TO_BUILD=X86                        \
		-DLLVM_OPTIMIZED_TABLEGEN=ON                       \
		-DLLVM_INSTALL_UTILS=ON                            \
		-DLLVM_CCACHE_BUILD=ON                             \
		-DBUILD_SHARED_LIBS=ON                             \
		-DLLVM_ENABLE_RTTI=ON                              \
		-DLLVM_PARALLEL_LINK_JOBS=8                        \
		-DCMAKE_BUILD_TYPE=Release                         \
		-DLLVM_ENABLE_PROJECTS="clang;lldb;compiler-rt"    \
		-DLLVM_BINUTILS_INCDIR=/usr/local/include   	   \
		-DCMAKE_C_COMPILER=clang                           \
		-DCMAKE_CXX_COMPILER=clang++                       \
	&& cmake --build .

qjs :
	$(MAKE) -C quickjs
	cd quickjs/.obj \
	&&  gcc -g -shared -fPIC -flto -o quickjs.so  \
	    cutils.o libbf.o libregexp.o libunicode.o \
		quickjs-libc.o qjsc.o qjscalc.o quickjs.o \
		-o quickjs.so


FLAGS=--cxxflags --ldflags --system-libs --libs core orcjit native

lljit : JitDriver.h JitDriver.cpp
	$(CXXCOMP) -g JitDriver.cpp `$(LLVMCONFIG) $(FLAGS)` -o LLJIT 

clean :
	-rm LLJIT 
