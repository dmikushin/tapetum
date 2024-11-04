#ifndef SERIALIZE_WRITER_H
#define SERIALIZE_WRITER_H

#include "SerializeWriterAdapter.h"

#include "clang/Frontend/CompilerInstance.h"

class SerializeWriter : public SerializeWriterAdapter
{
  // The writer shall have its own AST traversal and error reporting.
  clang::CompilerInstance &CI;

public:
  SerializeWriter(clang::CompilerInstance &CI);

  virtual ~SerializeWriter() {}

  virtual void serializeFunction(const std::string& fromArg, clang::Stmt *S) override;
  virtual void serializeStruct(const std::string& fromArg, const std::string& toArg, clang::Stmt* S) override;
};

#endif // SERIALIZE_WRITER_H
