#include <fstream>
#include <sstream>
#include "BibleData/VersePerLineFile.h"

namespace BIBLE_DATA
{
    std::optional<VersePerLineFile> VersePerLineFile::Parse(const std::filesystem::path& filepath)
    {
        // MAKE SURE THE FILE CAN BE OPENED.
        std::ifstream verse_per_line_file(filepath);
        if (!verse_per_line_file.is_open())
        {
            return std::nullopt;
        }

        // PARSE EACH LINE FROM THE FILE.
        std::vector<BibleVerse> verses;

        std::string line;
        while (std::getline(verse_per_line_file, line))
        {
            // PARSE OUT THE COMPONENTS OF THE LINE.
            BibleVerse verse;

            std::istringstream line_text(line);
            std::getline(line_text, verse.Book, '|');

            std::string chapter_number;
            std::getline(line_text, chapter_number, '|');
            verse.ChapterNumber = std::stoi(chapter_number);

            std::string verse_number;
            std::getline(line_text, verse_number, '|');
            verse.VerseNumber = std::stoi(verse_number);

            std::getline(line_text, verse.Text, '~');
            verse.Text.erase(0, 1);

            verses.push_back(verse);
        }

        return VersePerLineFile{ .Verses = verses };
    }
}
