#pragma once

#include <optional>
#include <filesystem>
#include <map>
#include <string>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA::FILES
{
    /// A raw file holding Bible data.
    /// This single class exists to provide an single way to access data
    /// that can be parsed from various file formats.
    class BibleDataFile
    {
    public:
        // PARSING.
        static std::optional<BibleDataFile> ParseVersePerLineFile(const std::string& translation_name, const std::filesystem::path& filepath);
        static std::optional<BibleDataFile> ParseOsisXmlFile(const std::string& translation_name, const std::filesystem::path& filepath);

        /// PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The name of the translation.
        std::string TranslationName = "";
        /// The verses parsed from the file.
        std::vector<BibleVerse> Verses = {};
        /// Books mapped by ID.
        std::map<BibleBookId, BibleBook> BooksById = {};
    };
}
