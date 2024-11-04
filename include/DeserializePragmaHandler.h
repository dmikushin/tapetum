#ifndef DESERIALIZE_PRAGMA_HANDLER_H
#define DESERIALIZE_PRAGMA_HANDLER_H

#include "DeserializeWriterAdapter.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Pragma.h"

class DeserializePragmaHandler : public clang::PragmaHandler {
  clang::CompilerInstance &CI;
  DeserializeWriterAdapter &Writer;
public:
  DeserializePragmaHandler(clang::CompilerInstance &CI, DeserializeWriterAdapter &Writer);
  void HandlePragma(clang::Preprocessor &PP, 
                    clang::PragmaIntroducer Introducer,
                    clang::Token &FirstToken) override;
};

#endif // DESERIALIZE_PRAGMA_HANDLER_H

