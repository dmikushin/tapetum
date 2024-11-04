#ifndef DESERIALIZE_CODE_WRITER_H
#define DESERIALIZE_CODE_WRITER_H

class DeserializeCodeWriter
{
public :
  DeserializeCodeWriter(clang::CompilerInstance &CI);

  void deserializeFunction(const std::string& fromArg, Stmt *S);
  void deserializeStruct(const std::string& fromArg, const std::string& toArg, Stmt* S);
}

#endif // DESERIALIZE_CODE_WRITER_H
