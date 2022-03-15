FLAGS=--cxxflags --ldflags --system-libs --libs core orcjit native

LLJIT : JitDriver.h JitDriver.cpp
	${CXXCOMP} -g JitDriver.cpp `${LLVMCONFIG} ${FLAGS}` -o LLJIT 

clean :
	-rm LLJIT 
