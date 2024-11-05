#ifndef SERIALIZE_WRITER_TEST_H
#define SERIALIZE_WRITER_TEST_H

#include "SerializeWriterAdapter.h"

#include "clang/Frontend/CompilerInstance.h"

class SerializeWriterTest : public SerializeWriterAdapter
{
  // The writer shall have its own AST traversal and error reporting.
  clang::CompilerInstance &CI;

public:
  SerializeWriterTest(clang::CompilerInstance &CI);

  virtual ~SerializeWriterTest() {}

  virtual void serializeFunction(const std::string& toArg, clang::Stmt *S) override;
  virtual void serializeStruct(const std::string& toArg, const std::string& fromArg, clang::Stmt* S) override;
};

#endif // SERIALIZE_WRITER_TEST_H
