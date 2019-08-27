#pragma once

#include <vector>
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// A single chapter in a book of the Bible.
    class BibleChapter
    {
    public:
        /// The number of the chapter.
        unsigned int Number = 0;
        /// The verses in the chapter.
        std::vector<BibleVerse> Verses = {};
    };
}
