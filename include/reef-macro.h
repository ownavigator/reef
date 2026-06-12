#pragma once

/*
 * Concatenate two tokens
 */

#define REEF_CAT_IMPL(__x, __y) __x##__y
#define REEF_CAT(__x, __y) REEF_CAT_IMPL(__x, __y)

#ifndef __COUNTER__
#error "reef-macro.h: __COUNTER__ is not supported by this compiler"
#endif

/*
 * Produce a unique symbol
 */

#define REEF_SYM(__base) REEF_CAT(__base, REEF_CAT(__COUNTER__, __LINE__))