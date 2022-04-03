#include "llvm/ADT/StringMap.h"
#include "llvm/ExecutionEngine/Orc/DebugUtils.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ObjectTransformLayer.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"

#include <string>

#include "JitDriver.h"

#define DEBUG_TYPE "orc"

using namespace llvm;
using namespace llvm::orc;

ExitOnError ExitOnErr;
cl::opt<std::string> InputFilename(cl::Positional,
                                   cl::desc("<input file>"),
                                   cl::Required);

cl::opt<bool> LLVMDebug("dump-llvm-debug",
                              cl::desc("Output llvm debug information"), cl::Optional,
                              cl::init(false));

cl::opt<bool> DisableGenerators("disable-generators",
                              cl::desc("Disable all symbol generators"), cl::Optional,
                              cl::init(false));

cl::opt<bool> DumpJITdObjects("dump-jitted-objects",
                              cl::desc("dump jitted objects"), cl::Optional,
                              cl::init(true));

cl::opt<bool> ActivateCFI("activate-cfi",
                              cl::desc("Make Control Flow Integrity Active"), cl::Optional,
                              cl::init(false));

cl::opt<std::string> DumpDir("dump-dir",
                             cl::desc("directory to dump objects to"),
                             cl::Optional, cl::init("."));

cl::opt<std::string> DumpFileStem("dump-file-stem",
                                  cl::desc("Override default dump names"),
                                  cl::Optional, cl::init(""));

int main(int argc, char *argv[]) {
  // Boilerplate initialization of LLVM.
  InitLLVM X(argc, argv);

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  cl::ParseCommandLineOptions(argc, argv, "LLJITDumpObjects");
  ExitOnErr.setBanner(std::string(argv[0]) + ": ");
  
  if (LLVMDebug) {
  	DebugFlag = true;
  }

  // Creates fresh IR from the provided code.
  std::string FileSuffix = ".ll";
  std::string FrontCmd = "$CCOMP -Iquickjs -g -emit-llvm -S programs/"+InputFilename+".c";
  if (ActivateCFI) {
	  FileSuffix = ".s";
	  FrontCmd = FrontCmd + " -fvisibility=hidden -fsanitize=cfi -flto";
  }
  system(FrontCmd.c_str());

  /*
         "Usage notes:\n"
         "  Use -debug-only=orc on debug builds to see log messages of objects "
         "being dumped\n"
         "  Specify -dump-dir to specify a dump directory\n"
         "  Specify -dump-file-stem to override the dump file stem\n"
         "  Specify -dump-jitted-objects=false to disable dumping\n";
  */

  auto J = ExitOnErr(LLJITBuilder().create());
 
  if (!DisableGenerators) {
    J->getMainJITDylib().addGenerator(
    ExitOnErr(DynamicLibrarySearchGenerator::GetForCurrentProcess(
	    J->getDataLayout().getGlobalPrefix()
	  )));
 
    J->getMainJITDylib().addGenerator(
    ExitOnErr(DynamicLibrarySearchGenerator::Load(
            "quickjs/.obj/quickjs.so",
            J->getDataLayout().getGlobalPrefix()
          )));
  } 
  
  if (DumpJITdObjects)
    J->getObjTransformLayer().setTransform(DumpObjects(DumpDir, DumpFileStem));

  LLVM_DEBUG(dbgs() << "Parsing IR from file\n");

  auto M = ExitOnErr(parseModuleFromFile(InputFilename+FileSuffix)); 
  
  LLVM_DEBUG(dbgs() << "Adding IR to Module\n");

  ExitOnErr(J->addIRModule(std::move(M)));
  
  LLVM_DEBUG(dbgs() << "Performing Function lookup\n");

  // Look up the JIT'd function then call it.
  auto jitMainSym = ExitOnErr(J->lookup("main"));
  
  auto jitMainAddr = jitMainSym.getAddress();
  
  LLVM_DEBUG(dbgs() << "Calling JIT function\n");

  auto jitMain = reinterpret_cast<void (*)(int,char**)>(jitMainAddr);
  jitMain(0, nullptr);

  system(("mkdir -p ./out/"+InputFilename).c_str());
  system(("cp "+InputFilename+"*.ll "+InputFilename+"*.o ./out/"+InputFilename).c_str());
  system("rm *.ll *.o *.s -rf");

  return 0;
}
