## Programs

This directory contains C programs for testing various mitigation strategies.
Some of the files were generated with QuickJS, others were written directly in 
C. Below is a description of the purpose of some of the files.

## Verification Files

### HelloWorld
This file is used to verify that the LLJIT works correctly, it acts as a test file for future analysis.

### JS-Hello 
This file is used to verify that the Javascript libraries are properly linked into the JIT and are working correctly. This should be used before running the other JS marked files.

## Test Files

### ControlFlowIntegrity
This file is used to test the control flow system within LLVM. The test is done by recieving a string that acts as a password, but which allows for an overflow that can redirect to the password succcess option even if the password should have failed.

To test this file, a specific compilation process must be used, which is as follows:
```
664-llvm/build/bin/clang -fvisibility=hidden -fsanitize=cfi -flto programs/ControlFlowIntegrity.c
perl -e 'print "a"x8 .  "\x80\x06\x40"' | ./a.out
```
The result will indicate an Illegal Instruction has been passed.

### ConstantFolding (Tests Constant Folding & Constant Blinding)
This file was created to test if the *constant folding* mitigation. It contains
a very basic addition opertation of two long integers. Running through the LLJIT
wrapper then using Clang to emit the assembly file (.s) from the LLVM IR file (.ll)
in the out directory reveals that no mitigation was present. 
From project root:
```
./LLJIT ConstantFolding
cd out/ConstantFolding
clang -S ConstantFolding.ll
vim ConstantFolding.s
```

### Large 
This file is used to test if allocations are split up or just done in successive pages. The file consists of a function with a sufficiently large amount of code as to require multiple pages of memory to hold the resulting compiled code in the JIT.
