#pragma once

#include <map>
#include "BibleData/BibleBookId.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// A single chapter in a book of the Bible.
    class BibleChapter
    {
    public:
        /// The book containing this chapter.
        BibleBookId Book = BibleBookId::INVALID;
        /// The number of the chapter.
        unsigned int Number = 0;
        /// The number of verses in the chapter.
        unsigned int VerseCount = 0;
    };
}
