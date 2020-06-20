#pragma once

#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleBookId.h"
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"

namespace BIBLE_DATA
{
    using BibleVerseIdsByWord = std::map<std::string, std::vector<BibleVerseId>>;
    using BibleVerseIdsByCharacterThenWord = std::map<char, BibleVerseIdsByWord>;

    /// A single translation of the Bible, holding the contents (verses) of the translation.
    class BibleTranslation
    {
    public:
        static std::shared_ptr<BibleTranslation> Populate(const std::vector<BibleVerse>& verses, const std::map<BibleBookId, BibleBook>& books);
        static std::shared_ptr<BibleTranslation> Create(
            const std::string& translation_name,
            const std::vector<BibleVerse>& verses,
            const std::map<BibleBookId, BibleBook>& books);

        std::vector<BibleVerse*> GetVerses(
            const BibleBookId book,
            const unsigned int chapter_number,
            const unsigned int starting_verse_number,
            const unsigned int ending_verse_number);

        std::string Name = "";
        /// A mapping of Bible verses by ID.
        std::map<BibleVerseId, BibleVerse> VersesById = {};
        std::map<BibleBookId, BibleBook> BooksById = {};

    private:
        BibleVerseIdsByCharacterThenWord BuildWordIndex();

        std::mutex Mutex = {};
        std::future<BibleVerseIdsByCharacterThenWord> WordIndexBeingLoaded = {};
        BibleVerseIdsByCharacterThenWord VersesByFirstLowercaseLetterThenImportantWord = {};
    };
}
