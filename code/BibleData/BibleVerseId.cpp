#include <cctype>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleVerseId.h"

namespace BIBLE_DATA
{
    /// Parses a verse range from the text.
    /// @param[in]  verse_range_text - The text from which to parse a verse range.
    /// @return The range, if successfully parsed; null otherwise.
    std::optional<std::pair<BibleVerseId, BibleVerseId>> BibleVerseId::ParseRange(const std::string& verse_range_text)
    {
        // PARSE OUT THE VERSE RANGE FROM THE TEXT.
        // For simplicity, we'll parse out everything in a single for loop,
        // which prevents having to keep track of certain indices.
        // In order to do this, we'll have to keep track which component
        // is currently being parsed, which should start with a book.
        bool parsing_first_book = true;
        std::string potential_first_book_name;
        bool parsing_first_book_chapter_number = false;
        std::string first_book_chapter_number;
        bool parsing_first_book_verse_number = false;
        std::string first_book_verse_number;

        BibleVerseId first_verse_id;

        bool parsing_verse_range_separator = false;

        bool parsing_second_book = false;
        std::string potential_second_book_name;
        bool parsing_second_book_chapter_number = false;
        std::string second_book_chapter_number;
        bool parsing_second_book_verse_number = false;
        std::string second_book_verse_number;

        BibleVerseId second_verse_id;

        std::size_t verse_range_text_length_in_characters = verse_range_text.size();
        for (std::size_t character_index = 0; character_index < verse_range_text_length_in_characters; ++character_index)
        {
            char character = verse_range_text[character_index];

            // PARSE PART OF THE FIRST BOOK IF APPLICABLE.
            if (parsing_first_book)
            {
                bool is_space = isspace(character);
                if (is_space)
                {
                    char next_character = 0;
                    std::size_t next_character_index = character_index + 1;
                    bool next_character_exists = (next_character_index < verse_range_text_length_in_characters);
                    if (next_character_exists)
                    {
                        next_character = verse_range_text[next_character_index];
                    }
                    bool next_character_is_number = next_character_exists && std::isdigit(next_character);
                    if (next_character_is_number)
                    {
                        // PARSE THE BOOK NAME.
                        first_verse_id.Book = BibleBook::GetId(potential_first_book_name);
                        bool book_valid = (BibleBookId::INVALID != first_verse_id.Book);
                        if (!book_valid)
                        {
                            return std::nullopt;
                        }

                        // MOVE TO PARSING THE NEXT COMPONENT.
                        // The next character is assumed to be a chapter number.
                        parsing_first_book = false;
                        parsing_first_book_chapter_number = true;
                    }
                    else
                    {
                        // ASSUME THE SPACE IS PART OF THE BOOK.
                        // This is to handle book names with spaces in them.
                        potential_first_book_name += character;
                    }
                }
                else
                {
                    // ASSUME THE CHARACTER IS PART OF THE BOOK.
                    potential_first_book_name += character;
                }

                continue;
            }

            if (parsing_first_book_chapter_number)
            {
                // SKIP OVER ANY SPACES.
                bool is_space = isspace(character);
                if (is_space)
                {
                    continue;
                }

                // CHECK IF THE CHARACTER IS A DIGIT.
                bool character_is_digit = std::isdigit(character);
                if (character_is_digit)
                {
                    first_book_chapter_number += character;
                }
                else
                {
                    // PARSE THE CHAPTER NUMBER.
                    first_verse_id.ChapterNumber = static_cast<unsigned int>(std::stoul(first_book_chapter_number));

                    // MOVE TO PARSING THE NEXT COMPONENT.
                    parsing_first_book_chapter_number = false;
                    parsing_first_book_verse_number = true;
                }

                continue;
            }

            if (parsing_first_book_verse_number)
            {
                // CHECK IF THE CHARACTER IS A DIGIT.
                bool character_is_digit = std::isdigit(character);
                if (character_is_digit)
                {
                    first_book_verse_number += character;

                    std::size_t next_character_index = character_index + 1;
                    bool next_character_exists = (next_character_index < verse_range_text_length_in_characters);
                    if (!next_character_exists)
                    {
                        // PARSE THE VERSE NUMBER.
                        first_verse_id.VerseNumber = static_cast<unsigned int>(std::stoul(first_book_verse_number));

                        // The second verse is initialized to a copy of the first verse ID
                        // to handle the case where a single verse is specified.
                        second_verse_id = first_verse_id;
                    }
                }
                else
                {
                    // PARSE THE VERSE NUMBER.
                    first_verse_id.VerseNumber = static_cast<unsigned int>(std::stoul(first_book_verse_number));

                    // MOVE TO PARSING THE NEXT COMPONENT.
                    parsing_first_book_verse_number = false;
                    parsing_verse_range_separator = true;

                    // The second verse is initialized to a copy of the first verse ID
                    // to handle the case where a single verse is specified.
                    second_verse_id = first_verse_id;
                }

                continue;
            }

            if (parsing_verse_range_separator)
            {
                // SWITCH TO PARSING THE BOOK IF A VALID CHARACTER WAS FOUND.
                bool is_valid_book_character = std::isalnum(character);
                if (is_valid_book_character)
                {
                    parsing_verse_range_separator = false;
                    parsing_second_book = true;
                }

                // No continue here to let the character drop down to the next condition.
            }

            if (parsing_second_book)
            {
                bool is_space = isspace(character);
                if (is_space)
                {
                    char next_character = 0;
                    std::size_t next_character_index = character_index + 1;
                    bool next_character_exists = (next_character_index < verse_range_text_length_in_characters);
                    if (next_character_exists)
                    {
                        next_character = verse_range_text[next_character_index];
                    }
                    bool next_character_is_number = next_character_exists && std::isdigit(next_character);
                    if (next_character_is_number)
                    {
                        // PARSE THE BOOK NAME.
                        second_verse_id.Book = BibleBook::GetId(potential_second_book_name);
                        bool book_valid = (BibleBookId::INVALID != second_verse_id.Book);
                        if (!book_valid)
                        {
                            return std::nullopt;
                        }

                        // MOVE TO PARSING THE NEXT COMPONENT.
                        // The next character is assumed to be a chapter number.
                        parsing_second_book = false;
                        parsing_second_book_chapter_number = true;
                    }
                    else
                    {
                        // ASSUME THE SPACE IS PART OF THE BOOK.
                        // This is to handle book names with spaces in them.
                        potential_second_book_name += character;
                    }
                }
                else
                {
                    // ASSUME THE CHARACTER IS PART OF THE BOOK.
                    potential_second_book_name += character;
                }

                continue;
            }

            if (parsing_second_book_chapter_number)
            {
                // SKIP OVER ANY SPACES.
                bool is_space = isspace(character);
                if (is_space)
                {
                    continue;
                }

                // CHECK IF THE CHARACTER IS A DIGIT.
                bool character_is_digit = std::isdigit(character);
                if (character_is_digit)
                {
                    second_book_chapter_number += character;
                }
                else
                {
                    // PARSE THE CHAPTER NUMBER.
                    second_verse_id.ChapterNumber = static_cast<unsigned int>(std::stoul(second_book_chapter_number));

                    // MOVE TO PARSING THE NEXT COMPONENT.
                    parsing_second_book_chapter_number = false;
                    parsing_second_book_verse_number = true;
                }

                continue;
            }

            if (parsing_second_book_verse_number)
            {
                // CHECK IF THE CHARACTER IS A DIGIT.
                bool character_is_digit = std::isdigit(character);
                if (character_is_digit)
                {
                    second_book_verse_number += character;

                    std::size_t next_character_index = character_index + 1;
                    bool next_character_exists = (next_character_index < verse_range_text_length_in_characters);
                    if (!next_character_exists)
                    {
                        // PARSE THE VERSE NUMBER.
                        second_verse_id.VerseNumber = static_cast<unsigned int>(std::stoul(second_book_verse_number));
                    }
                }
                else
                {
                    // PARSE THE VERSE NUMBER.
                    second_verse_id.VerseNumber = static_cast<unsigned int>(std::stoul(second_book_verse_number));

                    // FINISH PARSING THE REFERENCES.
                    parsing_second_book_verse_number = false;
                }

                continue;
            }
        }

        return std::make_pair(first_verse_id, second_verse_id);
    }
}
