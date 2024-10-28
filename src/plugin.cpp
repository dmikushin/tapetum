#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

#include "SerializePragmaHandler.h"
#include "DeserializePragmaHandler.h"

namespace {

class TapetumASTConsumer : public clang::ASTConsumer {
public:
  clang::PPCallbacks *PPC;
  SerializePragmaHandler SPH;
  DeserializePragmaHandler DPH;

  explicit TapetumASTConsumer(clang::CompilerInstance &CI) {
    auto &PP = CI.getPreprocessor();
    PP.AddPragmaHandler(&SPH);
    PP.AddPragmaHandler(&DPH);
  }
};

class TapetumPluginASTAction : public clang::PluginASTAction {
public:
  TapetumPluginASTAction() : clang::PluginASTAction() {}

  TapetumPluginASTAction(TapetumPluginASTAction const &) = delete;

  TapetumPluginASTAction &operator=(TapetumPluginASTAction const &) = delete;

private:
  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;  // Assume the arguments are fine
  }

  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override {
    return std::make_unique<TapetumASTConsumer>(CI);
  }

  clang::PluginASTAction::ActionType getActionType() override {
    return clang::PluginASTAction::ActionType::AddBeforeMainAction;
  }
};

}

static clang::FrontendPluginRegistry::Add<TapetumPluginASTAction>
X("tapetum", "Enable native C/C++ serialization and deserialization");

