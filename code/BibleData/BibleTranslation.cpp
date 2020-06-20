#include <unordered_set>
#include "BibleData/BibleTranslation.h"

namespace BIBLE_DATA
{
    /// Populates a Bible translation from the provided verses.
    /// @param[in]  verses - The verses included in the translation.
    std::shared_ptr<BibleTranslation> BibleTranslation::Populate(const std::vector<BibleVerse>& verses, const std::map<BibleBookId, BibleBook>& books)
    {
        auto translation = std::make_shared<BibleTranslation>();

#if 1
        // ADD ALL VERSES TO THE TRANSLATION.
        for (const auto& verse : verses)
        {
            translation->VersesById[verse.Id] = verse;
        }
#endif
        translation->BooksById = books;

        return translation;
    }

    /// @todo
    std::shared_ptr<BibleTranslation> BibleTranslation::Create(
        const std::string& translation_name,
        const std::vector<BibleVerse>& verses,
        const std::map<BibleBookId, BibleBook>& books)
    {
        auto translation = Populate(verses, books);
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

        BibleChapter& chapter = BooksById[book].ChaptersByNumber[chapter_number];

        auto chapter_starting_verse = chapter.VersesByNumber.lower_bound(starting_verse_number);
        auto chapter_after_ending_verse = chapter.VersesByNumber.upper_bound(ending_verse_number);

        for (auto id_and_verse = chapter_starting_verse; id_and_verse != chapter_after_ending_verse; ++id_and_verse)
        {
            verses.push_back(&id_and_verse->second);
        }

        return verses;
    }

    /// Builds an index for the Bible associating words with the verses containing them.
    /// @return A lookup from words to the verses containing them.
    BibleVerseIdsByCharacterThenWord BibleTranslation::BuildWordIndex()
    {
        std::lock_guard<std::mutex> lock(Mutex);

        // KJV occurrence counts are in comments beside each word.
        static const std::unordered_set<std::string> LOWERCASE_STOP_WORDS =
        {
            "a", // 8177
            "an",
            "and", // 51696
            "be", // 7013
            "for", // 8971
            "he", // 10420
            "him", // 6659
            "his", // 8473
            "i", // 8854
            "in", // 12667
            "is", // 6989
            "it", // 6129
            "not", // 6596
            "of", // 34617
            "that", // 12912
            "the", // 63924
            "them", // 6430
            "they", // 7376
            "to", // 13562
            "was",
            "which", // 4413
            "with", // 6012
        };

        // INDEX ALL OF THE VERSES BY WORD.
        BibleVerseIdsByCharacterThenWord verses_by_letter_then_word;
        std::map<std::string, std::vector<BibleVerse>> verses_by_word;
        for (const auto& id_and_verse : VersesById)
        {
            /// @todo   How to handle punctuation?
            const std::vector<Token>* current_verse_tokens = id_and_verse.second.GetTokens();
            for (const Token token : *current_verse_tokens)
            {
                bool is_word = (TokenType::WORD == token.Type);
                if (!is_word)
                {
                    continue;
                }

                std::string lowercase_word = token.Text;
                std::transform(
                    lowercase_word.begin(),
                    lowercase_word.end(),
                    lowercase_word.begin(),
                    [](const char character) { return static_cast<char>(std::tolower(character)); });

#if 0
                bool is_stop_word = LOWERCASE_STOP_WORDS.contains(lowercase_word);
                if (is_stop_word)
                {
                    continue;
                }
#endif


                char first_character = lowercase_word.front();
                verses_by_letter_then_word[first_character][lowercase_word].push_back(id_and_verse.first);
            }
        }

        return verses_by_letter_then_word;
    }
}
