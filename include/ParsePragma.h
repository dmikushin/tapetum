#ifndef PARSE_PRAGMA_H
#define PARSE_PRAGMA_H

#include "clang/AST/Stmt.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"

namespace tapetum {

void ParsePragmaWithStateMachine(clang::CompilerInstance &CI, clang::Preprocessor &PP,
	clang::Token& Tok, std::string& toArg, std::string& fromArg, clang::Stmt *&S);

} // namespace tapetum

#endif // PARSE_PRAGMA_H

