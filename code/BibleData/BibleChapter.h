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
        void AddVerse(const BibleVerse& verse)
        {
            VersesByNumber[verse.Id.VerseNumber] = verse;
        }

        /// The book containing this chapter.
        BibleBookId Book = BibleBookId::INVALID;
        /// The number of the chapter.
        unsigned int Number = 0;
        /// The number of verses in the chapter.
        unsigned int VerseCount = 0;

        std::map<unsigned int, BibleVerse> VersesByNumber = {};
    };
}
