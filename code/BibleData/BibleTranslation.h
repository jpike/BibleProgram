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
#include "BibleData/BibleVerseRange.h"
#include "BibleData/BibleWordIndex.h"

namespace BIBLE_DATA
{
    /// A single translation of the Bible, holding the contents (verses) of the translation.
    class BibleTranslation
    {
    public:
        std::vector<BibleVerse> GetVerses(const BibleVerseRange& verse_range) const;

        std::shared_ptr<BibleWordIndex> GetWordIndex();

        std::string Name = "";
        /// A mapping of Bible verses by ID.
        std::map<BibleVerseId, BibleVerse> VersesById = {};

    private:
        std::shared_ptr<BibleWordIndex> BuildWordIndex();

        std::mutex Mutex = {};
        std::shared_ptr<BibleWordIndex> WordIndex = {};
        std::future<std::shared_ptr<BibleWordIndex>> WordIndexBeingLoaded = {};
    };
}
