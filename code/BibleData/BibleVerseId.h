#pragma once

#include "BibleData/BibleBookId.h"

namespace BIBLE_DATA
{
    /// The unique ID for a Bible verse in the context of the entire Bible.
    class BibleVerseId
    {
    public:
        bool operator<(const BibleVerseId& rhs) const;

        /// The book containing the verse.
        BibleBookId Book = BibleBookId::INVALID;
        /// The chapter containing the verse.
        unsigned int ChapterNumber = 0;
        /// The number of the verse within the chapter.
        unsigned int VerseNumber = 0;
    };
}
