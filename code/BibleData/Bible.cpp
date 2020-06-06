#include <unordered_map>
#include "BibleData/Bible.h"

namespace BIBLE_DATA
{
    /// Adds data from the specified translation to the Bible.
    /// @param[in]  translation_name - The name of the translation.
    /// @param[in]  verses - The verses for the translation.
    void Bible::AddTranslation(const std::string& translation_name, const std::vector<BibleVerse>& verses, const std::map<BibleBookId, BibleBook>& books)
    {
        // MAKE SURE THE STRUCTURAL INFORMATION ABOUT THE TRANSLATION IS IN THE BIBLE.
        for (const auto& verse : verses)
        {
            // MAKE SURE THE BOOK EXISTS FOR THE VERSE.
            bool book_already_exists = BooksById.contains(verse.Id.Book);
            if (!book_already_exists)
            {
                BooksById[verse.Id.Book] = BibleBook{ .Id = verse.Id.Book };
            }

            // MAKE SURE THE BOOK CONTAINS THE CHAPTER FOR THE VERSE.
            BibleBook& book = BooksById[verse.Id.Book];
            bool book_contains_chapter = (verse.Id.ChapterNumber <= book.Chapters.size());
            if (!book_contains_chapter)
            {
                book.Chapters.push_back(BibleChapter{ .Book = verse.Id.Book, .Number = verse.Id.ChapterNumber });
            }

            // MAKE SURE THE CHAPTER CONTAINS THE VERSE.
            /// @todo   Revisit this index stuff...Might be better to just have a map to avoid off-by-one errors.
            unsigned int chapter_index = verse.Id.ChapterNumber - 1;
            BibleChapter& chapter = book.Chapters[chapter_index];
            bool chapter_contains_verse = (verse.Id.VerseNumber <= chapter.VerseCount);
            if (!chapter_contains_verse)
            {
                ++chapter.VerseCount;
            }
        }

        // ADD THE TRANSLATION ITSELF.
        BooksById = books;
        BibleTranslation translation = BibleTranslation::Populate(verses, books);
        TranslationsByName[translation_name] = translation;
    }

    /// Gets the specified verses.
    /// @param[in]  starting_verse_id - The ID of the first verse in the range to get.
    /// @param[in]  ending_verse_id - The ID of the last verse in the range to get.
    /// @return The specified verses.
    std::vector<BibleVerse> Bible::GetVerses(const BibleVerseId& starting_verse_id, const BibleVerseId& ending_verse_id) const
    {
        std::vector<BibleVerse> verses;

        auto current_translation = TranslationsByName.at("KJV");

        auto starting_verse = current_translation.VersesById.lower_bound(starting_verse_id);
        auto after_ending_verse = current_translation.VersesById.upper_bound(ending_verse_id);

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
            VersesByWord = BuildWordIndex();
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
    std::map<std::string, std::vector<BibleVerse>> Bible::BuildWordIndex() const
    {
        // INDEX ALL OF THE VERSES BY WORD.
        std::map<std::string, std::vector<BibleVerse>> verses_by_word;
        const BibleTranslation& current_translation = TranslationsByName.at("KJV");
        for (const auto& id_and_verse : current_translation.VersesById)
        {
            /// @todo   How to handle punctuation?
            const std::vector<Token>* current_verse_tokens = id_and_verse.second.GetTokens();
            for (const Token token : *current_verse_tokens)
            {
                verses_by_word[token.Text].push_back(id_and_verse.second);
            }
        }

        return verses_by_word;
    }
}
