# Tapetum: Clang plugin for native C/C++ serialization and deserialization

This is a Clang plugin, which adds serialization and deserialization capabilities into the given C/C++ code in the following two ways.

1. Intrinsics to serialize and deserialize any given structure:

```c
char* serialize(T* val);
T* deserialize(const char* json);
```

2. An instinsic to call a function from serialized arguments, that are deserialized on-the-fly:

```c
int ret = deserialize_call(function, const char* json);
```

3. An attribute to serialize the function arguments before or after the call:

```c
function(a,b) [[serialize(const char* json), before|after]];
```


## Building

The build & use requires Clang compilers:

```cmake
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=clang++-17 -DCMAKE_C_COMPILER=clang-17 ..
make
```


## Implementation

The type discovery must be spanned accross multiple compilation units. The complete and portable type discovery is unlikely possible due to the oddities in the system headers. Therefore, we aim at discovering the types that are about to be serialized or deserialized, lazily. This may introduce some performance overhead.

During the plugin execution the following is done:

1. The intrinsics are discovered, and the proper declarations are added, in order to support the use of the plain C language (no templates).
2. The entire type declarations AST is preserved in a special global variable.
3. An index of type declarations is built upon the binary startup
4. The requested type is being searched for and parsed recursively, upon a call to the serialization intrinsic

