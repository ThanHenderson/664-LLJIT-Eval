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
	&& ninja -j2

qjs :
	$(MAKE) -C quickjs


FLAGS=--cxxflags --ldflags --system-libs --libs core orcjit native

lljit : JitDriver.h JitDriver.cpp
	$(CXXCOMP) -g JitDriver.cpp `$(LLVMCONFIG) $(FLAGS)` -o LLJIT 

clean :
	-rm LLJIT 
