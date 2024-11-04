#include "DeserializeWriterTest.h"

using namespace clang;

DeserializeWriterTest::DeserializeWriterTest(clang::CompilerInstance &CI_) : CI(CI_) {}

void DeserializeWriterTest::deserializeFunction(const std::string& fromArg, Stmt *S)
{
}

void DeserializeWriterTest::deserializeStruct(const std::string& fromArg, const std::string& toArg, Stmt* S)
{
}

