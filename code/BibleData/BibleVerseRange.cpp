#include <cctype>
#include <exception>
#include <iostream>
#include <limits>
#include <regex>
#include "BibleData/Bible.h"
#include "BibleData/BibleBook.h"
#include "BibleData/BibleVerseRange.h"

namespace BIBLE_DATA
{
    /// Parses a verse range from the text.
    /// @param[in]  verse_range_text - The text from which to parse a verse range.
    /// @return The range, if successfully parsed; null otherwise.
    std::optional<BibleVerseRange> BibleVerseRange::Parse(const std::string& verse_range_text)
    {
#define REGEX 1
#if REGEX
        std::cout << "----------------------" << std::endl;
        std::cout << "Parsing: " << verse_range_text << std::endl;

        const std::string BOOK_NAME_REGEX("([A-za-z0-9 ]+)");
        const std::string CHAPTER_NUMBER_REGEX("(\\d+)");
        const std::string NUMERIC_RANGE_REGEX("((\\d+)-(\\d+))");
        const std::string CHAPTER_AND_VERSE_NUMBER_REGEX("((\\d+):(\\d+))");
        const std::string VERSE_NUMBER_REGEX("(\\d+)");

        const std::string OPTIONAL_WHITESPACE_REGEX = "\\s*";
        const std::string REQUIRED_WHITESPACE_REGEX = "\\s+";
        const std::string SINGLE_REFERENCE_REGEX = (
            OPTIONAL_WHITESPACE_REGEX +
            BOOK_NAME_REGEX + "?" +
            "(" +
            CHAPTER_NUMBER_REGEX + "|" + NUMERIC_RANGE_REGEX + "|" + CHAPTER_AND_VERSE_NUMBER_REGEX +
            ")"
            );
        const std::string FULL_REFERENCE_RANGE_REGEX = (
            SINGLE_REFERENCE_REGEX + "?" + 
            OPTIONAL_WHITESPACE_REGEX + "-" + OPTIONAL_WHITESPACE_REGEX +
            SINGLE_REFERENCE_REGEX
            );
        std::regex verse_range_regex(FULL_REFERENCE_RANGE_REGEX);

        const std::string single_book_reference_regex =
            "(" +
            // First verse reference.
            BOOK_NAME_REGEX + "?" +
            REQUIRED_WHITESPACE_REGEX +
            CHAPTER_NUMBER_REGEX +
            // Optional stuff for first reference.
            "(" +
            // Optional dash followed by another chapter number.
            "-" + CHAPTER_NUMBER_REGEX +
            "|" +
            // Optional colon followed by verse number.
            ":" + VERSE_NUMBER_REGEX +
            ")?" +
            ")";

        verse_range_regex = std::regex(
            single_book_reference_regex +
            // Optional 2nd verse reference after a dash.
            "(" + "\\s*-\\s*" +
            single_book_reference_regex +
            ")?"
        );

        std::smatch verse_regex_matches;
        bool match_found = std::regex_match(verse_range_text, verse_regex_matches, verse_range_regex);
        if (match_found)
        {
#define DEBUG_PRINT 0
#if DEBUG_PRINT
            std::cout << verse_regex_matches.size() << " matches" << std::endl;
            for (auto& match : verse_regex_matches)
            {
                std::cout << match.str() << std::endl;
            }
#endif
            try
            {
                BibleVerseRange verse_range;

                constexpr std::size_t FIRST_BOOK_NAME_MATCH_INDEX = 2;
                std::string first_book_name = verse_regex_matches[FIRST_BOOK_NAME_MATCH_INDEX];
                BibleBookId first_book_id = BibleBook::GetId(first_book_name);
                verse_range.StartingVerse.Book = verse_range.EndingVerse.Book = first_book_id;

                constexpr std::size_t FIRST_CHAPTER_NUMBER_MATCH_INDEX = 3;
                std::string first_chapter = verse_regex_matches[FIRST_CHAPTER_NUMBER_MATCH_INDEX];
                unsigned int first_chapter_number = static_cast<unsigned int>(std::stoul(first_chapter));
                verse_range.StartingVerse.ChapterNumber = verse_range.EndingVerse.ChapterNumber = first_chapter_number;

                verse_range.StartingVerse.VerseNumber = 1;
                unsigned int chapter_index = first_chapter_number - 1;
                verse_range.EndingVerse.VerseNumber = BIBLE_DATA::Bible::BOOKS_BY_ID.at(first_book_id).VerseCountsByChapter.at(chapter_index);

                constexpr std::size_t FIRST_DELIMITER_NUMBER_MATCH_INDEX = 4;
                std::string first_delimiter_and_number = verse_regex_matches[FIRST_DELIMITER_NUMBER_MATCH_INDEX];
                if (!first_delimiter_and_number.empty())
                {
                    if (first_delimiter_and_number.starts_with('-'))
                    {
                        constexpr std::size_t SECOND_CHAPTER_NUMBER_MATCH_INDEX = 5;
                        std::string second_chapter = verse_regex_matches[SECOND_CHAPTER_NUMBER_MATCH_INDEX];
                        unsigned int second_chapter_number = static_cast<unsigned int>(std::stoul(second_chapter));
                        verse_range.EndingVerse.ChapterNumber = second_chapter_number;
                        unsigned int second_chapter_index = second_chapter_number - 1;
                        verse_range.EndingVerse.VerseNumber = BIBLE_DATA::Bible::BOOKS_BY_ID.at(first_book_id).VerseCountsByChapter.at(second_chapter_index);
                    }
                    else if (first_delimiter_and_number.starts_with(':'))
                    {
                        constexpr std::size_t FIRST_VERSE_NUMBER_MATCH_INDEX = 6;
                        std::string first_verse = verse_regex_matches[FIRST_VERSE_NUMBER_MATCH_INDEX];
                        unsigned int first_verse_number = static_cast<unsigned int>(std::stoul(first_verse));
                        verse_range.StartingVerse.VerseNumber = verse_range.EndingVerse.VerseNumber = first_verse_number;
                    }
                }

                constexpr std::size_t SECOND_BOOK_NAME_MATCH_INDEX = 9;
                std::string second_book_name = verse_regex_matches[SECOND_BOOK_NAME_MATCH_INDEX];
                if (!second_book_name.empty())
                {
                    BibleBookId second_book_id = BibleBook::GetId(second_book_name);
                    verse_range.EndingVerse.Book = second_book_id;
                    verse_range.EndingVerse.ChapterNumber = 1;
                    verse_range.EndingVerse.VerseNumber = 1;
                }

                constexpr std::size_t SECOND_CHAPTER_NUMBER_MATCH_INDEX = 10;
                std::string second_chapter = verse_regex_matches[SECOND_CHAPTER_NUMBER_MATCH_INDEX];
                if (!second_chapter.empty())
                {
                    unsigned int second_chapter_number = static_cast<unsigned int>(std::stoul(second_chapter));
                    verse_range.EndingVerse.ChapterNumber = second_chapter_number;
                    unsigned int second_chapter_index = second_chapter_number - 1;
                    verse_range.EndingVerse.VerseNumber = BIBLE_DATA::Bible::BOOKS_BY_ID
                        .at(verse_range.EndingVerse.Book).VerseCountsByChapter.at(second_chapter_index);
                }

                constexpr std::size_t SECOND_VERSE_NUMBER_MATCH_INDEX = 13;
                std::string second_verse = verse_regex_matches[SECOND_VERSE_NUMBER_MATCH_INDEX];
                if (!second_verse.empty())
                {
                    unsigned int second_verse_number = static_cast<unsigned int>(std::stoul(second_verse));
                    verse_range.EndingVerse.VerseNumber = second_verse_number;
                }

                return verse_range;
            }
            catch (const std::exception& exception)
            {
                std::cout << "Standard exception parsing " << verse_range_text << ": " << exception.what() << std::endl;
                return std::nullopt;
            }
            catch (...)
            {
                std::cout << "Unknown exception parsing " << verse_range_text << std::endl;
                return std::nullopt;
            }
        }
        else
        {
            return std::nullopt;
        }      

#else
        // PARSE OUT THE VERSE RANGE FROM THE TEXT.
        // For simplicity, we'll parse out everything in a single for loop,
        // which prevents having to keep track of certain indices.
        // In order to do this, we'll have to keep track which component
        // is currently being parsed, which should start with a book.
        bool parsing_first_book = true;
        std::string potential_first_book_name;
        bool parsing_first_book_chapter_number = false;
        std::string first_book_chapter_number = "0";
        bool parsing_first_book_verse_number = false;
        std::string first_book_verse_number = "0";

        BibleVerseId first_verse_id;

        bool parsing_verse_range_separator = false;

        bool parsing_second_book = false;
        std::string potential_second_book_name;
        bool parsing_second_book_chapter_number = false;
        std::string second_book_chapter_number = "0";
        bool parsing_second_book_verse_number = false;
        std::string second_book_verse_number = "0";

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

                        second_verse_id.Book = first_verse_id.Book;

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

                    std::size_t next_character_index = character_index + 1;
                    bool next_character_exists = (next_character_index < verse_range_text_length_in_characters);
                    if (!next_character_exists)
                    {
                        // PARSE THE VERSE NUMBER.
                        first_verse_id.ChapterNumber = static_cast<unsigned int>(std::stoul(first_book_chapter_number));

                        // The second verse is initialized to a copy of the first verse ID
                        // to handle the case where a single verse is specified.
                        second_verse_id = first_verse_id;
                    }
                }
                else
                {
                    // PARSE THE CHAPTER NUMBER.
                    first_verse_id.ChapterNumber = static_cast<unsigned int>(std::stoul(first_book_chapter_number));
                    second_verse_id.ChapterNumber = first_verse_id.ChapterNumber;

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

                    // No continue here to let the character drop down to the next condition.
                }
                else if ('-' == character)
                {
                    second_verse_id = first_verse_id;
                    parsing_verse_range_separator = false;
                    parsing_second_book = true;
                    continue;
                }
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

                    std::size_t next_character_index = character_index + 1;
                    bool next_character_exists = (next_character_index < verse_range_text_length_in_characters);
                    if (!next_character_exists)
                    {
                        // PARSE THE VERSE NUMBER.
                        second_verse_id.ChapterNumber = static_cast<unsigned int>(std::stoul(second_book_chapter_number));
                    }
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

        bool first_verse_number_set = (first_verse_id.VerseNumber > 0);
        if (!first_verse_number_set)
        {
            first_verse_id.VerseNumber = 1;
        }

        bool second_verse_number_set = (second_verse_id.VerseNumber > 0);
        if (!second_verse_number_set)
        {
            second_verse_id.VerseNumber = std::numeric_limits<unsigned int>::max();
        }

        BibleVerseRange verse_range = 
        {
            .StartingVerse = first_verse_id,
            .EndingVerse = second_verse_id
        };
        return verse_range;
#endif
    }

    /// Equality operator.
    /// @param[in]  rhs - The verse range on the right-hand-side to compare against.
    /// @return True if the verse ranges are equal; false if not.
    bool BibleVerseRange::operator==(const BibleVerseRange& rhs) const
    {
        bool equal = (
            StartingVerse == rhs.StartingVerse &&
            EndingVerse == rhs.EndingVerse);
        return equal;
    }

    /// Inequality operator.
    /// @param[in]  rhs - The verse range on the right-hand-side to compare against.
    /// @return True if the verse ranges aren't equal; false if they are equal.
    bool BibleVerseRange::operator!=(const BibleVerseRange& rhs) const
    {
        bool equal = (*this == rhs);
        return !equal;
    }
}
