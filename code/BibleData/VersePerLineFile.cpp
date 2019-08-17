#include <fstream>
#include <sstream>
#include "BibleData/VersePerLineFile.h"

namespace BIBLE_DATA
{
    /// Attempts to parse the file at the specified path.
    /// @param[in]  filepath - The path to the file to parse.
    /// @return The parsed verse-per-line file, if successful; std::nullopt otherwise.
    std::optional<VersePerLineFile> VersePerLineFile::Parse(const std::filesystem::path& filepath)
    {
        // MAKE SURE THE FILE CAN BE OPENED.
        std::ifstream verse_per_line_file(filepath);
        if (!verse_per_line_file.is_open())
        {
            // INDICATE THAT THE FILE FAILED TO BE PARSED.
            return std::nullopt;
        }

        // PARSE EACH LINE FROM THE FILE.
        std::vector<BibleVerse> verses;

        std::string line;
        while (std::getline(verse_per_line_file, line))
        {
            BibleVerse verse;

            // PARSE OUT THE BOOK NAME.
            // The format of each line is Book|Chapter#|Verse#| VerseText~
            constexpr char COMPONENT_SEPARATOR = '|';
            std::istringstream line_text(line);
            std::getline(line_text, verse.Book, COMPONENT_SEPARATOR);

            // PARSE OUT THE CHAPTER NUMBER.
            std::string chapter_number;
            std::getline(line_text, chapter_number, COMPONENT_SEPARATOR);
            verse.ChapterNumber = std::stoi(chapter_number);

            // PARSE OUT THE VERSE NUMBER.
            std::string verse_number;
            std::getline(line_text, verse_number, COMPONENT_SEPARATOR);
            verse.VerseNumber = std::stoi(verse_number);

            // PARSE OUT THE VERSE TEXT.
            constexpr char LINE_TERMINATOR = '~';
            std::getline(line_text, verse.Text, LINE_TERMINATOR);

            // A single space character is at the beginning of this last component.
            constexpr std::size_t START_INDEX = 0;
            constexpr std::size_t SPACE_CHARACTER_COUNT = 1;
            verse.Text.erase(START_INDEX, SPACE_CHARACTER_COUNT);

            // STORE THE PARSED VERSE.
            verses.push_back(verse);
        }

        // RETURN THE PARSED FILE.
        return VersePerLineFile{ .Verses = verses };
    }
}
