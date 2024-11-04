#ifndef SERIALIZE_PRAGMA_HANDLER_H
#define SERIALIZE_PRAGMA_HANDLER_H

#include "SerializeWriterAdapter.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Pragma.h"

class SerializePragmaHandler : public clang::PragmaHandler {
  clang::CompilerInstance &CI;
  SerializeWriterAdapter &Writer;
public:
  SerializePragmaHandler(clang::CompilerInstance &CI, SerializeWriterAdapter &Writer);
  void HandlePragma(clang::Preprocessor &PP, 
                    clang::PragmaIntroducer Introducer,
                    clang::Token &FirstToken) override;
};

#endif // SERIALIZE_PRAGMA_HANDLER_H

