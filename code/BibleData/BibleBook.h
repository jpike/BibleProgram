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

        void AddVerse(const BibleVerse& verse);

        /// The ID of the book.
        BibleBookId Id = BibleBookId::INVALID;
        std::vector<BibleChapter> Chapters = {};
        /// The chapters in the book by numeric ID (starting at 1).
        std::map<unsigned int, BibleChapter> ChaptersByNumber = {};
    };
}
