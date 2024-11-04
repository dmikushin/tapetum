#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

#include "SerializePragmaHandler.h"
#include "DeserializePragmaHandler.h"
#include "SerializeWriter.h"
#include "DeserializeWriter.h"
#include "SerializeWriterTest.h"
#include "DeserializeWriterTest.h"

#include <memory>

namespace {

template<typename TSerializeWriter, typename TDeserializeWriter>
class TapetumASTConsumer : public clang::ASTConsumer {
public:
  SerializePragmaHandler *SPH;
  DeserializePragmaHandler *DPH;
  std::unique_ptr<TSerializeWriter> SW;
  std::unique_ptr<TDeserializeWriter> DW;

  explicit TapetumASTConsumer(clang::CompilerInstance &CI) {
    auto &PP = CI.getPreprocessor();
    SW.reset(new TSerializeWriter(CI));
    DW.reset(new TDeserializeWriter(CI));
    SPH = new SerializePragmaHandler(CI, *dynamic_cast<SerializeWriterAdapter*>(SW.get()));
    DPH = new DeserializePragmaHandler(CI, *dynamic_cast<DeserializeWriterAdapter*>(DW.get()));
    PP.AddPragmaHandler(SPH);
    PP.AddPragmaHandler(DPH);
  }

  virtual ~TapetumASTConsumer() {}
};

template<typename TSerializeWriter, typename TDeserializeWriter>
class TapetumPluginASTAction : public clang::PluginASTAction {
public:
  TapetumPluginASTAction() : clang::PluginASTAction() {}

  TapetumPluginASTAction(TapetumPluginASTAction const &) = delete;

  TapetumPluginASTAction &operator=(TapetumPluginASTAction const &) = delete;

  virtual ~TapetumPluginASTAction() {}

private:
  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    // Assume the arguments are fine
    return true;
  }

  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override {
    return std::make_unique<TapetumASTConsumer<TSerializeWriter, TDeserializeWriter>>(CI);
  }

  clang::PluginASTAction::ActionType getActionType() override {
    return clang::PluginASTAction::ActionType::AddBeforeMainAction;
  }
};

}

static clang::FrontendPluginRegistry::Add<TapetumPluginASTAction<SerializeWriter, DeserializeWriter>>
X("tapetum", "Enable native C/C++ serialization and deserialization of structs and functions");

static clang::FrontendPluginRegistry::Add<TapetumPluginASTAction<SerializeWriterTest, DeserializeWriterTest>>
XTest("tapetum-test", "Enable native C/C++ serialization and deserialization of structs and functions");

