#pragma once

#define REEF_CAT_IMPL(__x, __y) __x##__y
/** Concatenates two tokens after full macro expansion. */
#define REEF_CAT(__x, __y) REEF_CAT_IMPL(__x, __y)

#if !defined(__COUNTER__)
#error "reef-macro.h: __COUNTER__ is not supported by this compiler"
#endif

/** Produces a unique identifier derived from `__base`. */
#define REEF_SYM(__base) REEF_CAT(__base, REEF_CAT(__COUNTER__, __LINE__))