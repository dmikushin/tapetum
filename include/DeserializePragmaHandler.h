#ifndef DESERIALIZE_PRAGMA_HANDLER_H
#define DESERIALIZE_PRAGMA_HANDLER_H

#include "clang/Lex/Pragma.h"

class DeserializePragmaHandler : public clang::PragmaHandler {
public:
  DeserializePragmaHandler() : PragmaHandler("deserialize") {}
  void HandlePragma(clang::Preprocessor &PP, 
                    clang::PragmaIntroducer Introducer,
                    clang::Token &FirstToken) override;
};

#endif // DESERIALIZE_PRAGMA_HANDLER_H

