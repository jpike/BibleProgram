#include <filesystem>
#include <iostream>
#include <optional>
#include "BibleData/VersePerLineFile.h"

int main()
{
    std::optional<BIBLE_DATA::VersePerLineFile> verse_per_line_file = BIBLE_DATA::VersePerLineFile::Parse("../data/SacredTexts/kjvdat.txt");
    if (verse_per_line_file)
    {
        std::cout << "Successful parse." << std::endl;
    }
    else
    {
        std::cout << "Failed parse." << std::endl;
    }

    return 0;
}
