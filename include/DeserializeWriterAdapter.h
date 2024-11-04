#ifndef DESERIALIZE_WRITER_ADAPTER_H
#define DESERIALIZE_WRITER_ADAPTER_H

#include "clang/AST/Stmt.h"
#include <string>

// Once the pragma is parsed, we want to perform the code generation for it.
// This class is a base class for all code generators that take the parsed
// pragma parameters and generate the new AST code according to it.
class DeserializeWriterAdapter
{
public :  
  virtual ~DeserializeWriterAdapter() {};

  // Generate AST function deserialization code for the pragma replacement at the same location.
  virtual void deserializeFunction(const std::string &fromArg, clang::Stmt *S) = 0;

  // Generate AST structure deserialization code for the pragma replacement at the same location.
  virtual void deserializeStruct(const std::string &fromArg, const std::string &toArg, clang::Stmt *S) = 0;
};

#endif // DESERIALIZE_WRITER_ADAPTER_H

