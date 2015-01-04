
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

//Set of tags to indentify supported compilers:
#define VISUAL_STUDIO (0)
#define GCC           (1)
#define CLANG         (2)

//Now some aliases:
#define MSVC VISUAL_STUDIO
#define GNUC GCC
#define LLVM CLANG

#if defined(_MSC_VER)
#define COMPILER(tag) ((tag) == VISUAL_STUDIO)
#elif defined(__GNUC__) && !defined(__llvm__)
#define COMPILER(tag) ((tag) == GCC)
#elif defined(__llvm__)
#define COMPILER(tag) ((tag) == CLANG)
#else
#error Unsupported compiler
#endif

#endif /* ENVIRONMENT_HPP */