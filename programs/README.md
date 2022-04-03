## Programs

This directory contains C programs for testing various mitigation strategies.
Some of the files were generated with QuickJS, others were written directly in 
C. Below is a description of the purpose of some of the files.


#### Constant folding
This file was created to test if the *constant folding* mitigation. It contains
a very basic addition opertation of two long integers. Running through the LLJIT
wrapper then using Clang to emit the assembly file (.s) from the LLVM IR file (.ll)
in the out directory reveals that no mitigation was present.
