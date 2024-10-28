#ifndef SERIALIZE_PRAGMA_HANDLER_H
#define SERIALIZE_PRAGMA_HANDLER_H

#include "clang/Lex/Pragma.h"

class SerializePragmaHandler : public clang::PragmaHandler {
public:
  SerializePragmaHandler() : PragmaHandler("serialize") {}
  void HandlePragma(clang::Preprocessor &PP, 
                    clang::PragmaIntroducer Introducer,
                    clang::Token &FirstToken) override;
};

#endif // SERIALIZE_PRAGMA_HANDLER_H

