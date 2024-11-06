#include "ParsePragma.h"

#include "clang/AST/ParentMapContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TokenKinds.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace clang;

namespace {

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

enum class State {
  INIT,
  EXPECT_IDENTIFIER,
  EXPECT_COLON,
  GET_VALUE,
  EXPECT_COMMA_OR_CLOSE,
  FINAL
};

} // namespace

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

// https://stackoverflow.com/a/40128699/4063520
static const clang::Stmt* getParentFunctionBody(ASTContext &Context, const Stmt *S) {
    while (true) {
        // Get parents of the statement.
        const auto& parents = Context.getParents(*S);
        if (parents.empty()) {
            llvm::errs() << "Can not find parent\n";
            return nullptr;
        }
        // TODO What if multiple parents?
        llvm::errs() << "find parent size=" << parents.size() << "\n";
        S = parents[0].get<Stmt>();
        if (!S)
            return nullptr;
        S->dump();
        if (isa<CompoundStmt>(S)) {
            // This represents a group of statements like { stmt stmt }
            return S;
        }
    } 
}

void tapetum::ParsePragmaWithStateMachine(clang::CompilerInstance &CI, Preprocessor &PP,
  Token& Tok, std::string& toArg, std::string& fromArg, Stmt *&S) {
  std::string identifier;
  State state = State::INIT;
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

  // Find the next code statement after pragma.
  SourceLocation EodLoc = Tok.getLocation();
  FindNextStatementVisitor Visitor(EodLoc);
  Visitor.TraverseDecl(CI.getASTContext().getTranslationUnitDecl());

  // Find the next code statement after pragma.
  S = Visitor.getNextStatement();
  if (!S) {
    PP.Diag(Tok, diag::err_expected) << "statement";
    return;
  }

  // Make sure the statement belongs to some function body.
  if (!getParentFunctionBody(CI.getASTContext(), S)) {
    PP.Diag(Tok, diag::err_expected) << "enclosing function body";
    return;
  }
}

