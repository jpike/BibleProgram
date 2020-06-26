#pragma once

#include <string>
#include <map>
#include <vector>
#include "BibleData/BibleBookId.h"
#include "BibleData/BibleChapter.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// A single book in the Bible.
    class BibleBook
    {
    public:
        static BibleBookId GetId(const std::string& book_name_or_abbreviation);
        static std::string FullName(const BibleBookId book_id);

        explicit BibleBook(const std::vector<unsigned int>& verse_counts_by_chapter) :
            VerseCountsByChapter(verse_counts_by_chapter)
        {}

        std::vector<unsigned int> VerseCountsByChapter = {};
    };
}
