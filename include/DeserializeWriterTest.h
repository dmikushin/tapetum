#ifndef DESERIALIZE_WRITER_TEST_H
#define DESERIALIZE_WRITER_TEST_H

#include "DeserializeWriterAdapter.h"

#include "clang/Frontend/CompilerInstance.h"

class DeserializeWriterTest : public DeserializeWriterAdapter
{
  // The writer shall have its own AST traversal and error reporting.
  clang::CompilerInstance &CI;

public:
  DeserializeWriterTest(clang::CompilerInstance &CI);

  virtual ~DeserializeWriterTest() {}

  virtual void deserializeFunction(const std::string& fromArg, clang::Stmt *S) override;
  virtual void deserializeStruct(const std::string& fromArg, const std::string& toArg, clang::Stmt* S) override;
};

#endif // DESERIALIZE_WRITER_TEST_H
