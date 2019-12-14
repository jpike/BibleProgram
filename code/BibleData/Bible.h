#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleTranslation.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// The Bible in a structured form.
    class Bible
    {
    public:
        void AddTranslation(const std::string& translation_name, const std::vector<BibleVerse>& verses);

        /// The books in the Bible, mapped by their IDs.
        /// This member variable defines the overall organizational structure, rather than content, of the Bible.
        std::map<BibleBookId, BibleBook> BooksById = {};
        /// A mapping of translation names to the content of the actual translations.
        std::unordered_map<std::string, BibleTranslation> TranslationsByName = {};
    };
}
