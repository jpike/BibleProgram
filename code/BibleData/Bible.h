#pragma once

#include <vector>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// The Bible in a structured form.
    class Bible
    {
    public:
        static Bible Populate(const std::vector<BibleVerse>& verses);

        /// The books in the Bible.
        std::vector<BibleBook> Books = {};
    };
}
