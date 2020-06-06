#pragma once

#include <string>
#include <optional>
#include <utility>
#include "BibleData/BibleBookId.h"

namespace BIBLE_DATA
{
    /// The unique ID for a Bible verse in the context of the entire Bible.
    class BibleVerseId
    {
    public:
        static std::optional<std::pair<BibleVerseId, BibleVerseId>> ParseRange(const std::string& verse_range_text);

        bool operator<(const BibleVerseId& rhs) const
        {
            if (Book < rhs.Book) return true;
            if (Book > rhs.Book) return false;

            // Books are equal...
            if (ChapterNumber < rhs.ChapterNumber) return true;
            if (ChapterNumber > rhs.ChapterNumber) return false;

            // Books and chapters are equal...
            bool this_verse_before_rhs_verse = (VerseNumber < rhs.VerseNumber);
            return this_verse_before_rhs_verse;
        }

        /// The book containing the verse.
        BibleBookId Book = BibleBookId::INVALID;
        /// The chapter containing the verse.
        unsigned int ChapterNumber = 0;
        /// The number of the verse within the chapter.
        unsigned int VerseNumber = 0;
    };
}
