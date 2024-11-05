#include "SerializeWriterTest.h"

using namespace clang;

SerializeWriterTest::SerializeWriterTest(clang::CompilerInstance &CI_) : CI(CI_) {}

void SerializeWriterTest::serializeFunction(const std::string& toArg, Stmt *S)
{
}

void SerializeWriterTest::serializeStruct(const std::string& toArg, const std::string& fromArg, Stmt* S)
{
}

