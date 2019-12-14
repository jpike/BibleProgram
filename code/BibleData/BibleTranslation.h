#pragma once

#include <map>
#include <vector>
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"

namespace BIBLE_DATA
{
    /// A single translation of the Bible, holding the contents (verses) of the translation.
    class BibleTranslation
    {
    public:
        static BibleTranslation Populate(const std::vector<BibleVerse>& verses);

        /// A mapping of Bible verses by ID.
        std::map<BibleVerseId, BibleVerse> VersesById = {};
    };
}
