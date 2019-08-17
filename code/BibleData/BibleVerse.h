#pragma once

#include <string>

/// Holds code related to accessing and manipulating underlying data about the Bible.
namespace BIBLE_DATA
{
    /// A single verse from the Bible.
    class BibleVerse
    {
    public:
        /// The name of the book that the verse is in.
        std::string Book = "";
        /// The chapter number within the book that the verse is in.
        unsigned int ChapterNumber = 0;
        /// The number of the verse within the chapter of its book.
        unsigned int VerseNumber = 0;
        /// The text of the verse.
        std::string Text = "";
    };
}
