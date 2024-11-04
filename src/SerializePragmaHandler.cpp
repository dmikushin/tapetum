#include "SerializePragmaHandler.h"

SerializePragmaHandler::SerializePragmaHandler(clang::CompilerInstance &CI_, SerializeWriterAdapter &Writer_) :
  PragmaHandler("serialize"), CI(CI_), Writer(Writer_) {}


void SerializePragmaHandler::HandlePragma(clang::Preprocessor &PP, 
                                          clang::PragmaIntroducer Introducer,
                                          clang::Token &FirstToken) {
  // TODO: Implement serialization logic
}

