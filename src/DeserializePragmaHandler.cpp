#include "DeserializePragmaHandler.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace clang;

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
                                            Token &FirstToken) {
  if (!FirstToken.getIdentifierInfo()->isStr("deserialize"))
    return;

  std::string toArg;
  std::string fromArg;
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
      if (Tok.getIdentifierInfo()->getName().str() == "to") {
        toArg = argValue;
      } else {
        fromArg = argValue;
      }
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

  // Find whether this is a struct or functional form of pragma:
  // if "to:" is not present, the pragma is expected to be functional.
  if (toArg.empty()) {
    // TODO Find the next code statement after pragma:

    // TODO Make sure the statement is a function call

    // TODO If it is not, the "to:" part must be present.
    PP.Diag(Tok, diag::err_expected) << "to";
    return;
  }

  llvm::outs() << "Deserializing to: " << toArg << ", from: " << fromArg
               << "\n";
}
