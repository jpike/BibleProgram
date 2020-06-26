#include <unordered_set>
#include "BibleData/BibleTranslation.h"

namespace BIBLE_DATA
{
    /// Populates a Bible translation from the provided verses.
    /// @param[in]  verses - The verses included in the translation.
    std::shared_ptr<BibleTranslation> BibleTranslation::Populate(const std::vector<BibleVerse>& verses)
    {
        auto translation = std::make_shared<BibleTranslation>();

        // ADD ALL VERSES TO THE TRANSLATION.
        for (const auto& verse : verses)
        {
            translation->VersesById[verse.Id] = verse;
        }

        return translation;
    }

    /// @todo
    std::shared_ptr<BibleTranslation> BibleTranslation::Create(
        const std::string& translation_name,
        const std::vector<BibleVerse>& verses)
    {
        auto translation = Populate(verses);
        translation->Name = translation_name;

        translation->WordIndexBeingLoaded = std::async(std::launch::async, [translation]() { return translation->BuildWordIndex(); });

        return translation;
    }

    std::vector<BibleVerse*> BibleTranslation::GetVerses(
        const BibleBookId book,
        const unsigned int chapter_number,
        const unsigned int starting_verse_number,
        const unsigned int ending_verse_number)
    {
        std::lock_guard<std::mutex> lock(Mutex);

        std::vector<BibleVerse*> verses;

        book;
        chapter_number;
        starting_verse_number;
        ending_verse_number;
#if 0
        BibleChapter& chapter = BooksById[book].ChaptersByNumber[chapter_number];

        auto chapter_starting_verse = chapter.VersesByNumber.lower_bound(starting_verse_number);
        auto chapter_after_ending_verse = chapter.VersesByNumber.upper_bound(ending_verse_number);

        for (auto id_and_verse = chapter_starting_verse; id_and_verse != chapter_after_ending_verse; ++id_and_verse)
        {
            verses.push_back(&id_and_verse->second);
        }
#endif

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
