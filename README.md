# CMPUT 664 Project - LLVM JIT Security Evaluation

### Clone the repo
```
git clone https://github.com/ThanHenderson/664-LLJIT-Eval.git
```
## Manual Install
### Initialize subdirectories (LLVM and quickjs)
```
git submodule init
git submodule update
```

### Build LLVM
```
make llvm
```
This can take an extreme amount of time, be patient.
Note that the LLVM build in the Makefile is set for X86 targets only. If building
for a different machine, the Makefile will need to be altered.

There are also variations of llvm that can be built based on the analysis you want to perform:
- Debug Info: `llvm-debug`
- Control Flow Integrity Support: `llvm-cfi`

### Build quickjs
```
make qjs
```

### Set environment variables
```
source init.sh
```

### Build LLJIT (creates LLJIT executable)
```
make
```

## Docker install
```
sudo docker build -t jiteval .
```

```
sudo docker run -it jiteval
```

### Running a program
```
./LLJIT program-name
```

where *program-name* is a file from the *programs* subdirectory.


## Programs

The test progams are in the *programs* subdirectory. Within that directory is a 
README file that has more information on the purpose and/or source of each file.
