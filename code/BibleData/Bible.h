#pragma once

#include <string>
#include <vector>
#include <map>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleTranslation.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// The Bible in a structured form.
    class Bible
    {
    public:
        void AddTranslation(const std::string& translation_name, const std::vector<BibleVerse>& verses, const std::map<BibleBookId, BibleBook>& books_by_id);

        /// The books in the Bible, mapped by their IDs.
        /// This member variable defines the overall organizational structure, rather than content, of the Bible.
        /// An ordered map is used for automatic sorting.
        std::map<BibleBookId, BibleBook> BooksById = {};
        /// A mapping of translation names to the content of the actual translations.
        /// An ordered, rather than unordered map, to support automatic alphabetic sorting.
        std::map<std::string, BibleTranslation> TranslationsByName = {};
    };
}
