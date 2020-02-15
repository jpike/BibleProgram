#pragma once

#include <optional>
#include <filesystem>
#include <map>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// A simple text file that contains one Bible verse per line.
    /// This specific format is based on the KJV data from http://sacred-texts.com/bib/osrc/index.htm.
    class VersePerLineFile
    {
    public:
        // PARSING.
        static std::optional<VersePerLineFile> Parse(const std::filesystem::path& filepath);

        /// PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The verses parsed from the file.
        std::vector<BibleVerse> Verses = {};

        std::map<BibleBookId, BibleBook> BooksById = {};
    };
}
