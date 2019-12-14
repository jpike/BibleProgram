#include "BibleData/BibleTranslation.h"

namespace BIBLE_DATA
{
    /// Populates a Bible translation from the provided verses.
    /// @param[in]  verses - The verses included in the translation.
    BibleTranslation BibleTranslation::Populate(const std::vector<BibleVerse>& verses)
    {
        BibleTranslation translation;

        // ADD ALL VERSES TO THE TRANSLATION.
        for (const auto& verse : verses)
        {
            translation.VersesById[verse.Id] = verse;
        }

        return translation;
    }
}
