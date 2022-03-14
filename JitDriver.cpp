#include "llvm/ADT/StringMap.h"
#include "llvm/ExecutionEngine/Orc/DebugUtils.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ObjectTransformLayer.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"

#include "JitDriver.h"

using namespace llvm;
using namespace llvm::orc;

namespace {

struct FnMap {
  const char *fName;
  const void *fAddr;
};

// This will need to be updated with any functions that are needed.
std::vector<FnMap> getFns(void)
{
  FnMap m[] = {
    {"printf", (void *)printf},
    {"malloc", (void *)malloc}
  };

  return std::vector<FnMap>(std::begin(m), std::end(m));
}

// Add functions in from library (SRTL in this case) that the JIT'd code can access.
Error addBuiltins(orc::LLJIT &jitInstance, const DataLayout &DL) {
  orc::SymbolMap M;
  orc::MangleAndInterner Mangle(jitInstance.getExecutionSession(), DL);
  // Register every symbol that can be accessed from the JIT'ed code.
  auto fns = getFns();
  for (auto fn : fns) {
    M[Mangle(fn.fName)] = JITEvaluatedSymbol(
        pointerToJITTargetAddress(fn.fAddr), JITSymbolFlags());
  }

  if (auto Err = (jitInstance.getMainJITDylib().define(absoluteSymbols(M))))
    return Err;

  return Error::success();
}

} // anonymous namespace


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

  outs()
      << "Usage notes:\n"
         "  Use -debug-only=orc on debug builds to see log messages of objects "
         "being dumped\n"
         "  Specify -dump-dir to specify a dump directory\n"
         "  Specify -dump-file-stem to override the dump file stem\n"
         "  Specify -dump-jitted-objects=false to disable dumping\n";

  auto J = ExitOnErr(LLJITBuilder().create());

  if (DumpJITdObjects)
    J->getObjTransformLayer().setTransform(DumpObjects(DumpDir, DumpFileStem));
 
  auto M = ExitOnErr(parseModuleFromFile(InputFilename));

  ExitOnErr(J->addIRModule(std::move(M)));
  
  if (auto Err = addBuiltins(*J, J->getDataLayout()))
      return 0;

  // Look up the JIT'd function, cast it to a function pointer, then call it.
  auto jitMainSym = ExitOnErr(J->lookup("main"));
  auto jitMainAddr = jitMainSym.getAddress();

  auto jitMain = reinterpret_cast<void (*)()>(jitMainAddr);
  jitMain();

  return 0;
}
