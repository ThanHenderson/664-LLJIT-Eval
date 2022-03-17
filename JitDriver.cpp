#include "llvm/ADT/StringMap.h"
#include "llvm/ExecutionEngine/Orc/DebugUtils.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ObjectTransformLayer.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"

#include <string>

#include "JitDriver.h"

using namespace llvm;
using namespace llvm::orc;

// Command line arguments
ExitOnError ExitOnErr;
cl::opt<std::string> InputFilename(cl::Positional,
                                   cl::desc("<input file>"),
                                   cl::Required);

cl::opt<bool> DumpJITdObjects("dump-jitted-objects",
                              cl::desc("dump jitted objects"), cl::Optional,
                              cl::init(true));

cl::opt<std::string> DumpDir("dump-dir",
                             cl::desc("directory to dump objects to"),
                             cl::Optional, cl::init(""));

cl::opt<std::string> DumpFileStem("dump-file-stem",
                                  cl::desc("Override default dump names"),
                                  cl::Optional, cl::init(""));

int main(int argc, char *argv[]) {
  // Initialize LLVM.
  InitLLVM X(argc, argv);

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  cl::ParseCommandLineOptions(argc, argv, "LLJITDumpObjects");
  ExitOnErr.setBanner(std::string(argv[0]) + ": ");

  // Creates fresh IR from the provided code
  std::string FrontCmd = "$CCOMP -Iquickjs -emit-llvm -S programs/"+InputFilename+".c";
  system(FrontCmd.c_str());

  /*
  outs()
      << "Usage notes:\n"
         "  Use -debug-only=orc on debug builds to see log messages of objects "
         "being dumped\n"
         "  Specify -dump-dir to specify a dump directory\n"
         "  Specify -dump-file-stem to override the dump file stem\n"
         "  Specify -dump-jitted-objects=false to disable dumping\n";
  */

  auto J = ExitOnErr(LLJITBuilder().create());
  
  J->getMainJITDylib().addGenerator(
    ExitOnErr(DynamicLibrarySearchGenerator::GetForCurrentProcess(
	    J->getDataLayout().getGlobalPrefix()
	  )));
 
  J->getMainJITDylib().addGenerator(
    ExitOnErr(DynamicLibrarySearchGenerator::Load(
            "quickjs/.obj/test.so",
            J->getDataLayout().getGlobalPrefix()
          )));
  
  if (DumpJITdObjects)
    J->getObjTransformLayer().setTransform(DumpObjects(DumpDir, DumpFileStem));
 
  auto M = ExitOnErr(parseModuleFromFile(InputFilename+".ll")); 

  ExitOnErr(J->addIRModule(std::move(M)));

  // Look up the JIT'd function, cast it to a function pointer, then call it.
  auto jitMainSym = ExitOnErr(J->lookup("main"));
  auto jitMainAddr = jitMainSym.getAddress();

  auto jitMain = reinterpret_cast<void (*)(int,char**)>(jitMainAddr);
  jitMain(0, nullptr);

  system("rm *.ll *.o -rf");

  return 0;
}
