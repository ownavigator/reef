#pragma once

namespace reef::impl {
/*
 * @brief Represents the kind of a token.
 */

enum Token_Kind {
  Token_Kind_Ident,
};

/*
 * @brief Represents a token in the source code.
 */

struct Token {
  Token_Kind kind;
  
};

} // namespace reef::impl