#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleTranslation.h"
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"
#include "BibleData/BibleVerseRange.h"

namespace BIBLE_DATA
{
    /// The Bible in a structured form.
    class Bible
    {
    public:
        /// The books in the Bible, mapped by their IDs.
        /// This only holds the high-level organizational/hierarchical metadata,
        /// rather than the contents of each book.
        /// This defines the overall organizational structure, rather than content, of the Bible.
        /// An ordered map is used for automatic sorting.
        static const std::map<BibleBookId, BibleBook> BOOKS_BY_ID;

        std::map<std::string, std::vector<BibleVerse>> BuildWordIndex();

        
        /// A mapping of translation names to the content of the actual translations.
        /// An ordered, rather than unordered map, to support automatic alphabetic sorting.
        std::map<std::string, std::shared_ptr<BibleTranslation>> TranslationsByName = {};

        std::map<
            char,
            std::map<std::string, std::vector<BibleVerseId>>
            > BibleVersesByFirstLowercaseLetterThenImportantWord = {};
    };
}
