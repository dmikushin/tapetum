#ifndef SERIALIZE_WRITER_ADAPTER_H
#define SERIALIZE_WRITER_ADAPTER_H

#include "clang/AST/Stmt.h"
#include <string>

// Once the pragma is parsed, we want to perform the code generation for it.
// This class is a base class for all code generators that take the parsed
// pragma parameters and generate the new AST code according to it.
class SerializeWriterAdapter
{
public :
  virtual ~SerializeWriterAdapter() {};

  // Generate AST function serialization code for the pragma replacement at the same location.
  virtual void serializeFunction(const std::string &toArg, clang::Stmt *S) = 0;

  // Generate AST structure serialization code for the pragma replacement at the same location.
  virtual void serializeStruct(const std::string &toArg, const std::string &fromArg, clang::Stmt *S) = 0;
};

#endif // SERIALIZE_WRITER_ADAPTER_H

