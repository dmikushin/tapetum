#include "DeserializePragmaHandler.h"

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

class FindNextStatementVisitor : public RecursiveASTVisitor<FindNextStatementVisitor> {
public:
  explicit FindNextStatementVisitor(SourceLocation pragmaLoc)
      : PragmaLoc(pragmaLoc), Found(false) {}

  bool VisitStmt(Stmt *S) {
    // Сheck if this is the next statement after the pragma.
    if (Found || S->getBeginLoc() <= PragmaLoc)
      return true;

    Found = true;
    NextStatement = S;
    
    // Stop traversing once we've found the statement.
    return false;
  }

  Stmt *getNextStatement() {
    if (Found) return NextStatement;
    
    return nullptr;
  }

private:
  SourceLocation PragmaLoc;
  bool Found;
  Stmt *NextStatement;
};

static std::string getArgumentValue(Preprocessor &PP, Token &Tok) {
  if (Tok.is(tok::string_literal)) {
    return Lexer::getSourceText(CharSourceRange::getTokenRange(
                                    Tok.getLocation(), Tok.getLocation()),
                                PP.getSourceManager(), PP.getLangOpts())
        .str();
  } else if (Tok.is(tok::identifier)) {
    return Tok.getIdentifierInfo()->getName().str();
  }
  return "";
}

enum class State {
  INIT,
  EXPECT_IDENTIFIER,
  EXPECT_COLON,
  GET_VALUE,
  EXPECT_COMMA_OR_CLOSE,
  FINAL
};

DeserializePragmaHandler::DeserializePragmaHandler(clang::CompilerInstance &CI_, DeserializeWriterAdapter &Writer_) :
  PragmaHandler("deserialize"), CI(CI_), Writer(Writer_) {}

bool ContainsFunctionCall(const Stmt* S) {
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
  std::string identifier;
  State state = State::INIT;

  Token Tok;
  while (state != State::FINAL) {
    PP.Lex(Tok);

    switch (state) {
    case State::INIT:
      if (!Tok.is(tok::l_paren)) {
        PP.Diag(Tok, diag::err_expected) << "(";
        return;
      }
      state = State::EXPECT_IDENTIFIER;
      break;
    case State::EXPECT_IDENTIFIER:
      if (Tok.is(tok::identifier)) {
        std::string argName = Tok.getIdentifierInfo()->getName().str();
        if (argName == "to" || argName == "from") {
          identifier = argName;
          state = State::EXPECT_COLON;
        } else {
          PP.Diag(Tok, diag::err_expected) << "to or from";
          return;
        }
      } else if (Tok.is(tok::r_paren)) {
        state = State::FINAL;
      } else {
        PP.Diag(Tok, diag::err_expected) << "identifier or )";
        return;
      }
      break;
    case State::EXPECT_COLON:
      if (!Tok.is(tok::colon)) {
        PP.Diag(Tok, diag::err_expected) << ":";
        return;
      }
      state = State::GET_VALUE;
      break;
    case State::GET_VALUE: {
      std::string argValue = getArgumentValue(PP, Tok);
      if (identifier == "to") {
        toArg = argValue;
      } else {
        fromArg = argValue;
      }
      identifier = "";
      state = State::EXPECT_COMMA_OR_CLOSE;
    } break;
    case State::EXPECT_COMMA_OR_CLOSE:
      if (!Tok.is(tok::comma)) {
        if (Tok.is(tok::r_paren)) {
          state = State::FINAL;
        } else {
          PP.Diag(Tok, diag::err_expected) << ", or )";
          return;
        }
      } else {
        state = State::EXPECT_IDENTIFIER;
      }
      break;
    case State::FINAL:
      break;
    }
  }

  // Make sure the "from:" part is always present.
  if (fromArg.empty()) {
    PP.Diag(Tok, diag::err_expected) << "from";
    return;
  }

  // Find the next code statement after pragma.
  SourceLocation EodLoc = Tok.getLocation();
  FindNextStatementVisitor Visitor(EodLoc);
  Visitor.TraverseDecl(CI.getASTContext().getTranslationUnitDecl());

  // Find the next code statement after pragma.
  Stmt *S = Visitor.getNextStatement();
  if (!S) {
    PP.Diag(Tok, diag::err_expected) << "statement";
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

