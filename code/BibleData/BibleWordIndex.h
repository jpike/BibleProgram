#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"

namespace BIBLE_DATA
{
    using BibleVerseIdsByWord = std::map<std::string, std::vector<BibleVerseId>>;
    using BibleVerseIdsByCharacterThenWord = std::map<char, BibleVerseIdsByWord>;

    /// An index of words in a Bible to support quick lookups.
    class BibleWordIndex
    {
    public:
        static std::shared_ptr<BibleWordIndex> Build(const std::map<BibleVerseId, BibleVerse>& verses_by_id);

        /// Counts for the number of times each word appears.
        std::map<std::string, unsigned int> OccurrenceCountsByLowercaseWord = {};
        BibleVerseIdsByCharacterThenWord VersesByFirstLowercaseLetterThenImportantWord = {};
    };
}
