#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/SourceMgr.h"


inline llvm::Error createSMDiagnosticError(llvm::SMDiagnostic &Diag) {
  using namespace llvm;
  std::string Msg;
  {
    raw_string_ostream OS(Msg);
    Diag.print("", OS);
  }
  return make_error<StringError>(std::move(Msg), inconvertibleErrorCode());
}

inline llvm::Expected<llvm::orc::ThreadSafeModule>
parseModuleFromFile(llvm::StringRef FileName) {
  using namespace llvm;
  auto Ctx = std::make_unique<LLVMContext>();
  SMDiagnostic Err;

  if (auto M = parseIRFile(FileName, Err, *Ctx))
    return orc::ThreadSafeModule(std::move(M), std::move(Ctx));

  return createSMDiagnosticError(Err);
}

