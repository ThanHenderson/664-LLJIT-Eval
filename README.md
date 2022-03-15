## CMPUT 664 Project - LLVM JIT Security Evaluation

### Clone the repo
```
git clone https://github.com/ThanHenderson/664-LLJIT-Eval.git
```

### Initialize subdirectories (LLVM and quickjs)
```
git submodule init
git submodule update
```

### Build LLVM
```
make llvm
```

### Build quickjs
```
make qjs
```

### Set environment variables
```
source init
```

### Build LLJIT (creates LLJIT executable)
```
make
```

### Running a program
```
./LLJIT <program-name>*
```

where <program-name> is a file from the *program* subdirectory. 
