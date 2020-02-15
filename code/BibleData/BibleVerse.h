#pragma once

#include <string>
#include <vector>
#include "BibleData/BibleVerseId.h"
#include "BibleData/Token.h"

/// Holds code related to accessing and manipulating underlying data about the Bible.
namespace BIBLE_DATA
{
    /// A single verse from the Bible.
    class BibleVerse
    {
    public:
        static std::vector<Token> Tokenize(const std::string& verse_text);
        const std::vector<Token>* GetTokens()
        {
            if (Tokens.empty())
            {
                Tokens = Tokenize(Text);
            }
            return &Tokens;
        }

        /// The ID of the verse.
        BibleVerseId Id = {};
        /// The text of the verse.
        std::string Text = "";

        /// \todo
        /// The verse text broken down into tokens.
        std::vector<Token> Tokens = {};
    };
}
