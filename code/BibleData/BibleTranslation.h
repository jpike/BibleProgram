#pragma once

#include <map>
#include <map>
#include <vector>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleBookId.h"
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"

namespace BIBLE_DATA
{
    /// A single translation of the Bible, holding the contents (verses) of the translation.
    class BibleTranslation
    {
    public:
        static BibleTranslation Populate(const std::vector<BibleVerse>& verses, const std::map<BibleBookId, BibleBook>& books);

        std::vector<BibleVerse*> GetVerses(
            const BibleBookId book,
            const unsigned int chapter_number,
            const unsigned int starting_verse_number,
            const unsigned int ending_verse_number);

        /// A mapping of Bible verses by ID.
        std::map<BibleVerseId, BibleVerse> VersesById = {};

        std::map<BibleBookId, BibleBook> BooksById = {};
    };
}
