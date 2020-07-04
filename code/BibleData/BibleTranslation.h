#pragma once

#include <map>
#include <string>
#include <vector>
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"
#include "BibleData/BibleVerseRange.h"

namespace BIBLE_DATA
{
    /// A single translation of the Bible, holding the contents (verses) of the translation.
    class BibleTranslation
    {
    public:
        std::vector<BibleVerse> GetVerses(const BibleVerseRange& verse_range) const;
        
        /// The name (abbreviation) of the translation.
        std::string Name = "";
        /// A mapping of Bible verses by ID.
        std::map<BibleVerseId, BibleVerse> VersesById = {};
    };
}
