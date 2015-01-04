
#ifndef SPECIFIERS_HPP
#define SPECIFIERS_HPP

#include "environment.hpp"

#if COMPILER(GCC) || COMPILER(CLANG)
	#define CONSTEXPR constexpr
	#define NOEXCEPT noexcept
#elif COMPILER(VISUAL_STUDIO)
	#define CONSTEXPR const
	#define NOEXCEPT throw()
#endif

#endif /* SPECIFIERS_HPP */