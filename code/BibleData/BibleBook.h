#pragma once

#include <string>
#include <vector>
#include "BibleData/BibleChapter.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// A single book in the Bible.
    class BibleBook
    {
    public:
        void Add(const BibleVerse& verse);

        /// The name (title) of the book.
        std::string Name = "";
        /// The chapters in the book.
        std::vector<BibleChapter> Chapters = {};
    };
}
