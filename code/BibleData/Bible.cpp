#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include "BibleData/Bible.h"

namespace BIBLE_DATA
{
    /// Adds the specified translation to the Bible.
    /// @param[in]  translation - The translation to add.
    void Bible::AddTranslation(const std::shared_ptr<BibleTranslation>& translation)
    {
        /// @todo   This is pretty slow.
        // MAKE SURE THE STRUCTURAL INFORMATION ABOUT THE TRANSLATION IS IN THE BIBLE.
        for (const auto& id_and_verse : translation->VersesById)
        {
            // MAKE SURE THE BOOK EXISTS FOR THE VERSE.
            bool book_already_exists = BooksById.contains(id_and_verse.first.Book);
            if (!book_already_exists)
            {
                BooksById[id_and_verse.first.Book] = BibleBook{ .Id = id_and_verse.first.Book };
            }

            // MAKE SURE THE BOOK CONTAINS THE CHAPTER FOR THE VERSE.
            BibleBook& book = BooksById[id_and_verse.first.Book];
            bool book_contains_chapter = (id_and_verse.first.ChapterNumber <= book.Chapters.size());
            if (!book_contains_chapter)
            {
                book.Chapters.push_back(BibleChapter{ .Book = id_and_verse.first.Book, .Number = id_and_verse.first.ChapterNumber });
            }

            // MAKE SURE THE CHAPTER CONTAINS THE VERSE.
            /// @todo   Revisit this index stuff...Might be better to just have a map to avoid off-by-one errors.
            unsigned int chapter_index = id_and_verse.first.ChapterNumber - 1;
            BibleChapter& chapter = book.Chapters[chapter_index];
            bool chapter_contains_verse = (id_and_verse.first.VerseNumber <= chapter.VerseCount);
            if (!chapter_contains_verse)
            {
                ++chapter.VerseCount;
            }
        }

        // ADD THE TRANSLATION ITSELF.
        BooksById = translation->BooksById;
        TranslationsByName[translation->Name] = translation;
    }

    /// Gets the specified verses.
    /// @param[in]  starting_verse_id - The ID of the first verse in the range to get.
    /// @param[in]  ending_verse_id - The ID of the last verse in the range to get.
    /// @return The specified verses.
    std::vector<BibleVerse> Bible::GetVerses(const BibleVerseId& starting_verse_id, const BibleVerseId& ending_verse_id) const
    {
        std::vector<BibleVerse> verses;

        const auto& current_translation = TranslationsByName.at("KJV");

        auto starting_verse = current_translation->VersesById.lower_bound(starting_verse_id);
        auto after_ending_verse = current_translation->VersesById.upper_bound(ending_verse_id);

        for (auto& id_and_verse = starting_verse; id_and_verse != after_ending_verse; ++id_and_verse)
        {
            verses.push_back(id_and_verse->second);
        }

        return verses;
    }

    /// Gets the verses containing the specified word.
    /// @param[in]  word - The word for which to get verses containing it.
    /// @return The verses containing the specified word.
    std::vector<BibleVerse> Bible::GetVerses(const std::string& word) const
    {
        // MAKE SURE THE WORD INDEX IS POPULATED.
        bool bible_verse_by_word_index_populated = !VersesByWord.empty();
        if (!bible_verse_by_word_index_populated)
        {
            //VersesByWord = BuildWordIndex();
        }

        // GET THE VERSES FOR THE SPECIFIED WORD.
        /// @todo   More advanced substring matching?
        auto word_and_verses_with_it = VersesByWord.find(word);
        bool verses_exist_with_word = (VersesByWord.cend() != word_and_verses_with_it);
        if (verses_exist_with_word)
        {
            return word_and_verses_with_it->second;
        }
        else
        {
            return {};
        }
    }

    /// Builds an index for the Bible associating words with the verses containing them.
    /// @return A lookup from words to the verses containing them.
    std::map<std::string, std::vector<BibleVerse>> Bible::BuildWordIndex()
    {
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
        std::map<std::string, std::vector<BibleVerse>> verses_by_word;
        const auto& current_translation = TranslationsByName.at("KJV");
        for (const auto& id_and_verse : current_translation->VersesById)
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
                BibleVersesByFirstLowercaseLetterThenImportantWord[first_character][lowercase_word].push_back(id_and_verse.first);
            }
        }

        return verses_by_word;
    }
}
