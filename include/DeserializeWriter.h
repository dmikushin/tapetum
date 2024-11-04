#ifndef DESERIALIZE_WRITER_H
#define DESERIALIZE_WRITER_H

#include "DeserializeWriterAdapter.h"

#include "clang/Frontend/CompilerInstance.h"

class DeserializeWriter : public DeserializeWriterAdapter
{
  // The writer shall have its own AST traversal and error reporting.
  clang::CompilerInstance &CI;

public:
  DeserializeWriter(clang::CompilerInstance &CI);

  virtual ~DeserializeWriter() {}

  virtual void deserializeFunction(const std::string& fromArg, clang::Stmt *S);
  virtual void deserializeStruct(const std::string& fromArg, const std::string& toArg, clang::Stmt* S);
};

#endif // DESERIALIZE_WRITER_H
