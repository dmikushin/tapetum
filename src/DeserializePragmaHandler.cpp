#include "DeserializePragmaHandler.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace clang;

static std::string getArgumentValue(Preprocessor &PP, Token &Tok) {
    if (Tok.is(tok::string_literal)) {
        return Lexer::getSourceText(CharSourceRange::getTokenRange(Tok.getLocation(), Tok.getLocation()), PP.getSourceManager(), PP.getLangOpts()).str();
    } else if (Tok.is(tok::identifier)) {
        return Tok.getIdentifierInfo()->getName().str();
    }
    return "";
}

// Here we parse the two supported forms of the "deserialize" pragma:
// * Structure form:
//   #pragma deserialize(from: json, to: val)
//   struct Val val;
//
// * Function form: 
//   #pragma deserialize(from: json)
//   int ret = function(a, b);
//
void DeserializePragmaHandler::HandlePragma(Preprocessor &PP, PragmaIntroducer Introducer, Token &FirstToken) {
    if (FirstToken.getIdentifierInfo()->isStr("deserialize")) {
        std::string toArg;
        std::string fromArg;
        bool beforeFlag = false;

        // Check for opening parenthesis
        PP.Lex(Tok);
        if (!Tok.is('(')) {
            PP.Diag(Tok, diag::err_expected) << "(";
            return;
        }

        while (true) {
            PP.Lex(Tok);
            if (Tok.is(tok::identifier)) {
                std::string argName = Tok.getIdentifierInfo()->getName().str();
                if (argName == "to" || argName == "from") {
                    // Check for colon
                    PP.Lex(Tok);
                    if (!Tok.is(':')) {
                        PP.Diag(Tok, diag::err_expected) << ":";
                        return;
                    }

                    // Get argument value
                    PP.Lex(Tok);
                    std::string argValue = getArgumentValue(PP, Tok);
                    if (argName == "to") {
                        toArg = argValue;
                    } else {
                        fromArg = argValue;
                    }
                } else {
                    // Unknown argument name
                    PP.Diag(Tok, diag::err_expected) << "to or from";
                    return;
                }
            } else if (Tok.is(')')) {
                break;
            } else if (!Tok.is(',')) {
                // Expected comma or closing parenthesis
                PP.Diag(Tok, diag::err_expected) << ", or )";
                return;
            }
        }

        // Make "to:" is always present.
        if (toArg.empty()) {
            PP.Diag(Tok, diag::err_expected) << "to";
            return;
        }

		// Find whether this is a struct or functional form of pragma:
		// if "from:" is not present, the pragma is expected to be functional.
        if (fromArg.empty()) {
            // TODO Find the next code statement after pragma:
            
            PP.Diag(Tok, diag::err_expected) << "from";
            return;
        }

        llvm::outs() << "Deserializing to: " << toArg << ", from: " << fromArg << "\n";
    } else {
        // If it's not a deserialize pragma, report an error
        PP.Diag(FirstToken, diag::err_unknown_pragma);
    }
}
