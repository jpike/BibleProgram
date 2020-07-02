#pragma once

#include <optional>
#include <string>
#include "BibleData/BibleVerseId.h"

namespace BIBLE_DATA
{
    /// An inclusive range of Bible verses.
    class BibleVerseRange
    {
    public:
        static std::optional<BibleVerseRange> Parse(const std::string& verse_range_text);

        bool operator==(const BibleVerseRange& rhs) const;
        bool operator!=(const BibleVerseRange& rhs) const;

        /// The starting verse in the range.
        BibleVerseId StartingVerse = {};
        /// The ending verse in the range.
        BibleVerseId EndingVerse = {};
    };
}
