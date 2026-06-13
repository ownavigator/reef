# Coding conventions

This is a C-style C++ codebase. C++ is here for templates, operator overloading,
and zero-data vtable interfaces, and everything else stays close to C: plain data
structs, free functions, explicit memory, no exceptions, no standard library.
Most of the rules below fall out of that, so when one surprises you, start there.

## Data, not objects

Structs hold data. That is their whole job. Every struct is plain old data, with
no access specifiers, because `private` and `protected` buy us nothing and cost
us clarity. Write `struct`, never `class`.

Behaviour lives in free functions, not methods. A function that operates on a
type is named after it, `<type>_<verb>`, and takes the instance as its first
pointer argument:

```cpp
isize wide_str_at(Wide_Str const* str, usize idx, i32* out_codepoint);
```

The name of that first parameter is up to you, so pick whatever reads best for
the function at hand. Templates and operator overloading are both fair game when
they earn their keep.

The single place we allow inheritance is a zero-data abstract interface, a
vtable, like `Allocator`. An inherited struct contributes no fields of its own,
ever. We reach for a vtable rather than a struct full of function pointers for
one concrete reason: the compiler optimises virtual dispatch better than it
optimises an indirect call through a stored pointer. That payoff is the only
thing that bends the anti-OOP rule. Small RAII helpers such as `Defer` are the
other sanctioned exception, and they each have one narrow job.

## Naming

Types are written in `Ada_Case`, with every word capitalised and joined by
underscores: `Wide_Str`, `Heap_Allocator`, `Defer`. Functions, variables, and
struct members are `snake_case`. Constants at file or static scope are
`SCREAMING_CASE`, like `ALLOCATOR` and `JS_SOURCE`.

Macros are the one place where casing carries meaning. A lowercase `reef_` macro
is one that could almost be a function and only resorts to the preprocessor for
scope or lifetime reasons. It should read like an ordinary call, the way
`reef_defer` and `reef_err_defer` do. An uppercase `REEF_` macro does something a
function genuinely cannot, such as pasting tokens together or minting a unique
symbol, as `REEF_CAT` and `REEF_SYM` do. The capitals are a warning that real
preprocessor machinery is at work, so read with care.

## Memory and allocators

Nothing in library code calls `malloc` or `new` behind your back. Any function
that might allocate takes an `Allocator*`, and by convention it is the last
parameter. Allocation is therefore always visible and always the caller's to
control.

A type that owns memory keeps only the memory. `Wide_Str` is a buffer and a
length and nothing more. It does not stash the allocator it was made with, so
`drop` asks the caller to hand back a compatible one. This is a little more
typing at the call site and it is what keeps owned types plain data.

## The lifecycle verbs

A small vocabulary covers how things are born and how they die, and we stick to
it so a function's name tells you its shape.

- `new` returns a `T` by value. Use it only when construction cannot fail.
- `init` takes a `T*` and returns an error state. Use it when construction can
  fail.
- `from` builds a `T` out of something else, as `wide_str_from_utf8` does. It
  follows the `new` shape when the conversion cannot fail and the `init` shape
  when it can.
- `drop` tears a `T*` down, no matter how it was created.

If construction can fail, it has to take the `init` shape. A by-value return
cannot report failure without a result type, and we do not use result types.

When a name needs a qualifier, the verb still comes first and the qualifier
trails it. The buffer-borrowing constructor is `wide_str_new_nocopy`, never
`wide_str_nocopy_new`. The pattern is always `<type>_<verb>` with any extra
words after that.

## Error handling

We assume an operation failed until it proves otherwise. Code is written for the
unhappy path first, and success is something you earn by the end of the function.

There are no exceptions. Every error is handled in the open, and the build makes
that non-negotiable by compiling with `-fno-exceptions` and `-fno-rtti`. See
`COPTS` in `BUILD.bazel`. We also avoid result types, since they are not free.

One invariant is worth stating on its own because it is easy to forget. When a
function reports an error and also writes through a `T*` output parameter, that
output is not guaranteed to be initialised on the error path, and reading it is
undefined behaviour. Touch an output parameter only after you have confirmed the
call succeeded.

Output parameters live at the end of the parameter list and are named `out_`,
like `out_codepoint`, so you can see at a glance what a function writes back. The
exception is `init` and `from`, where the thing being constructed comes first and
is named `uninit`, a reminder that the memory behind the pointer is not live yet
at the moment you receive it.

When a function acquires resources before it can fail, we unwind with `defer` and
a single `ok` flag rather than nested cleanup:

```cpp
bool ok = false;
u16* wide_ptr = alloc<u16>(allocator, wide_len);
reef_err_defer(!ok, dealloc(allocator, wide_ptr, wide_len));
// steps that may return false early
ok = true;
return ok;
```

`ok` starts false because we expect the worst. Each resource is guarded by a
`reef_err_defer(!ok, ...)` the moment it is acquired, so any early `return false`
rolls it back. `ok` becomes true exactly once, right before the final return. For
cleanup that should always run, such as a scratch buffer that never escapes, use
a plain `reef_defer`.

## Integers

Always use fixed-width integers. The aliases live in `reef-base.h`: `u8`, `u16`,
`u32`, `u64`, their signed counterparts `i8` through `i64`, and `usize` and
`isize` for pointer-sized values. The builtin types `int`, `long`, `short`,
`unsigned`, bare `char` as a number, and `size_t` do not appear in our own code.
They show up only at the boundary with an external API that insists on them, for
instance when handing a `char const*` to simdutf.

For type aliases reach for `typedef`. Only use `using` when you actually need an
alias template.

## The standard library

We do not use the C++ standard library. It is linked at all only because simdutf
pulls it in, and that is not an invitation to start leaning on it ourselves.

Owned data is a raw buffer, a length, and an explicit `Allocator`, the way
`Wide_Str` is put together. No `std::vector`, no `std::string`, no iostreams for
state we own. This is what keeps types plain data and memory under the
allocator's control. Freestanding headers such as `cstdint` and `type_traits`,
and thin shims over the C library, are fine when you need them. It is the heavy
runtime machinery we turn down.

## Files, headers, and includes

Headers use `#pragma once`, never include guards. Every header is named with a
`reef-` prefix, the way v8 does it, so the project namespace rides along in the
filename instead of forcing a nested `reef/` directory. The header is
`reef-str.h`, not `reef/str.h`. Headers live in `include/`; the matching
implementation is a plain `str.cc` in `src/`, one pair per concept. The `coral`
binary sits in its own `coral/` directory.

Includes come in three groups: the file's own header first, then our vendored
dependencies, then system headers. Both project headers and vendored
dependencies are quoted, and only genuine system or freestanding headers use
angle brackets:

```cpp
#include "reef-str.h"
#include "simdutf.h"
#include "utf8proc.h"
#include <cassert>
```

The quoting on dependencies is load-bearing, not taste. simdutf and utf8proc are
vendored and patched, utf8proc with our own hand-added utf16 support, and neither
ships native Bazel support. rules_cc compiles with `-iquote`, so those headers
resolve only through quoted includes. An angle-bracket include either fails to
find the vendored copy or, worse, silently picks up a system installation with a
different API, which is how you end up with a build that works on one machine and
a language server that complains about methods that plainly exist.

## Comments

Comments are always block comments. Use `/** ... */` for documentation and
`/* ... */` for notes inside a function body. We do not use `//` line comments.
The lone exception is the namespace-closing idiom, `} // namespace reef`, which
the editor writes for us and which would only look strange as a block comment.

Documentation is Doxygen and autobrief: the first sentence is the summary. We
comment the pivot points, not everything. A `/** ... */` block belongs on every
struct and enum, where one decisive sentence about what the type is for and how
it is meant to be used earns its keep, and a trailing `/**< ... */` belongs on
every struct member and enumerator. Fields are worth annotating; a name plus a
unit or an ownership note packs more than the type alone.

Functions do not get a doc block by default. A name built from the lifecycle
verbs and the type already tells you the shape, so most free functions read fine
bare, and a wall of `/** ... */` blocks down a header is noise we do not want. A
function earns a block only when it carries something the declaration cannot,
like ownership transfer, units, error behaviour, or a lifecycle precondition.
Macros are the other place to reach for a comment, since the preprocessor is
surprising by nature. When you do comment, document what a caller cannot read off
the declaration and do not narrate what the code already says plainly. A
forty-line file has no business carrying three hundred lines of Doxygen, and
internal statics and local logic earn a comment only when they are genuinely
surprising.

Write comments as complete sentences in English, with the first word capitalized
and a closing period, even for a one-line field note. English because the engine
is read internationally, and full sentences because a fragment forces the reader
to reconstruct what you meant. A comment should say why the code does what it does
or what it does at a higher level, not restate the line beneath it. `len` does not
need a comment that says "the length"; it needs one that says what the length
counts.

Two small habits. Do not hedge inside parentheses. If something matters, write it
as a sentence; if it does not, cut it. The "this (and also that) and the other"
half-measure helps no one. And keep comments plain ASCII with no emoji. Non-ASCII
text belongs only in deliberate test data, like the emoji in coral's JavaScript
fixture that exercises UTF-16 handling.

## Formatting

`.clang-format` is the final word on layout, so run it and move on. The style
started from Google's and then departed where we disagreed.

Braces are Allman: every brace sits on its own line, for functions, control flow,
structs, namespaces, and enums alike. Templates come through this cleanly, with
the `template <...>` prefix untouched and short one-liners left compact, which is
the part GNU style gets wrong. Always brace a body, even a single-statement `if`
or `for`. Indentation is four spaces and never tabs. Lines stop at eighty
columns. Pointers bind to the left, so `u8* ptr`.

We write east const, with `const` to the right of what it qualifies: `u8 const*`
and `Wide_Str const* str`, never a leading `const`. Leading const reads awkwardly
and is a quirk of the grammar, while east const keeps one simple rule, that
`const` always binds to its left.

Casts are C-style: `(u16*)ptr`, `(usize)rc`. The named C++ casts buy us nothing
here and only add noise.

From Google we kept the eighty-column limit, left-bound pointers, and the general
shape of the layout. Where we parted ways: `Ada_Case` types instead of
`CamelCase`, four-space indentation, Allman braces, and east const.
