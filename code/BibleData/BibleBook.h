#pragma once

#include <string>
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

        /// The ID of the book.
        BibleBookId Id = BibleBookId::INVALID;
        /// The chapters in the book.
        std::vector<BibleChapter> Chapters = {};
    };
}
