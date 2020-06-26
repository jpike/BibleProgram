#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleTranslation.h"
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"

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

        void AddTranslation(const std::shared_ptr<BibleTranslation>& translation);
        std::vector<BibleVerse> GetVerses(const BibleVerseId& starting_verse_id, const BibleVerseId& ending_verse_id) const;
        std::vector<BibleVerse> GetVerses(const std::string& word) const;
        std::map<std::string, std::vector<BibleVerse>> BuildWordIndex();

        
        /// A mapping of translation names to the content of the actual translations.
        /// An ordered, rather than unordered map, to support automatic alphabetic sorting.
        std::map<std::string, std::shared_ptr<BibleTranslation>> TranslationsByName = {};

        /// Verses by word.  Populated dynamically as needed.
        mutable std::map<std::string, std::vector<BibleVerse>> VersesByWord = {};

        std::map<
            char,
            std::map<std::string, std::vector<BibleVerseId>>
            > BibleVersesByFirstLowercaseLetterThenImportantWord = {};
    };
}
