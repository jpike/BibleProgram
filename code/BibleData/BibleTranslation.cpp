#include <unordered_set>
#include "BibleData/BibleTranslation.h"

namespace BIBLE_DATA
{
    /// Gets the specified verses.
    /// @param[in]  verse_range - The range of verses to get.
    /// @return The specified verses.
    std::vector<BibleVerse> BibleTranslation::GetVerses(const BibleVerseRange& verse_range) const
    {
        std::vector<BibleVerse> verses;

        auto starting_verse = VersesById.lower_bound(verse_range.StartingVerse);
        auto after_ending_verse = VersesById.upper_bound(verse_range.EndingVerse);

        for (auto& id_and_verse = starting_verse; id_and_verse != after_ending_verse; ++id_and_verse)
        {
            verses.push_back(id_and_verse->second);
        }

        return verses;
    }

    std::shared_ptr<BibleWordIndex> BibleTranslation::GetWordIndex()
    {
        std::lock_guard<std::mutex> lock(Mutex);

        if (WordIndex)
        {
            return WordIndex;
        }

        if (WordIndexBeingLoaded.valid())
        {
            WordIndex = WordIndexBeingLoaded.get();
            return WordIndex;
        }
        else
        {
            return nullptr;
        }
    }

    /// Builds an index for the Bible associating words with the verses containing them.
    /// @return A lookup from words to the verses containing them.
    std::shared_ptr<BibleWordIndex> BibleTranslation::BuildWordIndex()
    {
        std::lock_guard<std::mutex> lock(Mutex);

        std::shared_ptr<BibleWordIndex> word_index = BibleWordIndex::Build(VersesById);
        return word_index;
    }
}
