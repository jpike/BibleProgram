#pragma once

namespace BIBLE_DATA
{
    /// The different types of tokens as verse may be broken down into.
    enum class TokenType
    {
        /// The token is invalid.
        INVALID = 0,
        /// The token is a space or some sequence of whitespace.
        SPACE,
        /// The token is some kind of punctuation character or sequence of characters.
        PUNCTUATION,
        /// The token is a single word.
        WORD
    };
}
