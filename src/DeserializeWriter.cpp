#include "DeserializeWriter.h"

using namespace clang;

DeserializeWriter::DeserializeWriter(clang::CompilerInstance &CI_) : CI(CI_) {}

void DeserializeWriter::deserializeFunction(const std::string& fromArg, Stmt *S)
{
}

void DeserializeWriter::deserializeStruct(const std::string& fromArg, const std::string& toArg, Stmt* S)
{
}

