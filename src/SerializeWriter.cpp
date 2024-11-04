#include "SerializeWriter.h"

using namespace clang;

SerializeWriter::SerializeWriter(clang::CompilerInstance &CI_) : CI(CI_) {}

void SerializeWriter::serializeFunction(const std::string& fromArg, Stmt *S)
{
}

void SerializeWriter::serializeStruct(const std::string& fromArg, const std::string& toArg, Stmt* S)
{
}

