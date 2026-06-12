# CLAUDE.md

This file provides guidance to AI coding agents working in this repository. It
is intentionally tool-agnostic; any assistant operating on this codebase should
follow it.

## What this is

reef is a JavaScript engine, built as a Bazel `cc_library` (`//:reef`). `coral`
(`//coral:coral`) is a standalone driver binary that links the library and is
currently used as a scratch frontend for exercising the engine. The engine is at
an early stage: the present code covers UTF-16 string handling and an allocator
abstraction, with tokenization in progress (see git history).

## Build and run

This is a Bazel (bzlmod) project. There is no test suite yet.

```sh
bazel build //...            # build the library and coral
bazel run //coral:coral      # run the driver binary
./commands.sh                # refresh compile_commands.json for clangd
```

`./commands.sh` wraps `bazel run @hedron_compile_commands//:refresh_all`. Run it
after changing build files or adding sources so the language server stays
accurate.

## Architecture

The library is a flat set of concept-per-pair modules. Each public header is
`include/reef-<name>.h` and pairs with an implementation `src/<name>.cc`. Headers
are aggregated by the `//include:hdrs` filegroup and sources by `//src:srcs`;
`//:reef` bundles them with the vendored dependencies.

Load-bearing pieces to understand before editing:

- **`reef-base.h`** is the foundation every file builds on: the fixed-width
  integer aliases (`u8`..`u64`, `i8`..`i64`, `usize`/`isize`) and the `Defer` /
  `reef_defer` / `reef_err_defer` scope-exit machinery used for cleanup.
- **`reef-allocator.h`** defines the `Allocator` vtable interface and the
  `alloc` / `dealloc` / `copy` templates. Allocation is never implicit: every
  function that allocates takes an `Allocator*`, and owned types do not store
  their allocator. `heap_allocator()` returns the process-wide instance.
- **`reef-str.h` / `str.cc`** is `Wide_Str`, a UTF-16 buffer plus length that the
  caller owns. It is the working example of the whole memory model: construct
  with `wide_str_from_utf8` (fallible, fills an `uninit` out-param, returns a
  bool), iterate with `wide_str_at`, release with `wide_str_drop`.
- **`reef-module.h`** is the intended center of gravity (`Module` = a source file
  and its compilation state: tokens, AST, caches), currently a stub.

### Vendored dependencies and the include gotcha

simdutf and utf8proc are vendored as patched forks via `git_override` in
`MODULE.bazel`. utf8proc carries hand-added UTF-16 support (`utf8proc_iterate_utf16`)
that the upstream library does not have. Because of this, **dependency headers
must be included with quotes** (`"simdutf.h"`, `"utf8proc.h"`), not angle
brackets. `rules_cc` compiles with `-iquote`, so an angle-bracket include either
fails to resolve or silently picks up a divergent system copy.

## Conventions

Read `docs/conventions.md` before making non-trivial changes and follow it. It is
the authoritative style guide, and this codebase deviates from common C++ habits
in ways that will trip up an edit if you have not read it.
