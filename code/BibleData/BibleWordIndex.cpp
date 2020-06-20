#include <algorithm>
#include <unordered_set>
#include "BibleData/BibleWordIndex.h"

namespace BIBLE_DATA
{
    std::shared_ptr<BibleWordIndex> BibleWordIndex::Build(const std::map<BibleVerseId, BibleVerse>& verses_by_id)
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
        std::shared_ptr<BibleWordIndex> word_index = std::make_shared<BibleWordIndex>();
        std::map<std::string, std::vector<BibleVerse>> verses_by_word;
        for (const auto& id_and_verse : verses_by_id)
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
                word_index->VersesByFirstLowercaseLetterThenImportantWord[first_character][lowercase_word].push_back(id_and_verse.first);
                (word_index->OccurrenceCountsByLowercaseWord[lowercase_word])++;
            }
        }
        return word_index;
    }
}
