#include "SerializeWriterTest.h"

using namespace clang;

SerializeWriterTest::SerializeWriterTest(clang::CompilerInstance &CI_) : CI(CI_) {}

void SerializeWriterTest::serializeFunction(const std::string& fromArg, Stmt *S)
{
}

void SerializeWriterTest::serializeStruct(const std::string& fromArg, const std::string& toArg, Stmt* S)
{
}

