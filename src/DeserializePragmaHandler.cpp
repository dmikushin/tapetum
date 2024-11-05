#include "DeserializePragmaHandler.h"
#include "ParsePragma.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace tapetum;

static bool ContainsFunctionCall(const Stmt* S) {
    if (isa<CallExpr>(S)) {
        return true;
    }
    for (Stmt::const_child_iterator I = S->child_begin(), E = S->child_end(); I != E; ++I) {
        if (ContainsFunctionCall(*I)) {
            return true;
        }
    }
    return false;
}

DeserializePragmaHandler::DeserializePragmaHandler(clang::CompilerInstance &CI_, DeserializeWriterAdapter &Writer_) :
  PragmaHandler("deserialize"), CI(CI_), Writer(Writer_) {}

// Here we parse the two supported forms of the "deserialize" pragma:
// * Structure form:
//     #pragma deserialize(from: json, to: val)
//     struct Val val;
//
// * Function form:
//     #pragma deserialize(from: json)
//     int ret = function(a, b);
//
void DeserializePragmaHandler::HandlePragma(Preprocessor &PP,
                                            PragmaIntroducer Introducer,
                                            Token &StartTok) {
  std::string toArg;
  std::string fromArg;

  Token Tok;
  Stmt *S = nullptr;
  ParsePragmaWithStateMachine(CI, PP, Tok, toArg, fromArg, S);

  // Make sure the "from:" part is always present.
  if (fromArg.empty()) {
    PP.Diag(Tok, diag::err_expected) << "from";
    return;
  }

  // TODO Make sure the statement belongs to some function body.
  // Check if the statement is part of a function body
  //llvm::outs << S->getParent() << "\n";
  /*auto parent = S->getParent();
  while (parent && !isa<FunctionDecl>(parent)) {
    parent = parent->getParent();
  }*/

  // Find whether this is a struct or functional form of pragma:
  // if "to:" is not present, the pragma is expected to be functional.
  if (toArg.empty()) {
    // Make sure the statement contains a function call
    if (!S || !!ContainsFunctionCall(S)) {
      PP.Diag(Tok, diag::err_expected) << "function call after pragma or to:";
      return;
    }

    // Generate AST function deserialization code for the pragma replacement at the same location.
    Writer.deserializeFunction(fromArg, S);
  }
  else {
    // TODO Make sure the to: argument refers to an existing identifier,
    // and this identifier refers to the name of a structure instance.
    
    
    // Generate AST structure deserialization code for the pragma replacement at the same location.
    Writer.deserializeStruct(fromArg, toArg, S);
  }
}

