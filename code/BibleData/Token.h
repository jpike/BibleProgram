#pragma once

#include <string>
#include "BibleData/TokenType.h"

namespace BIBLE_DATA
{
    /// A single token that may be part of a Bible verse (https://en.wikipedia.org/wiki/Lexical_analysis#Tokenization).
    /// Verses are tokenized into words and punctuation in order to make it easier to manipulate text on an
    /// individual word basis.
    class Token
    {
    public:
        /// The type of token.
        TokenType Type = TokenType::INVALID;
        /// The text of the token.
        std::string Text = "";
    };
}
