#pragma once

#include <optional>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include "BibleData/BibleBook.h"
#include "BibleData/BibleTranslation.h"
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA::FILES
{
    /// A raw file holding Bible data.
    /// This single class exists to provide a single way to access data
    /// that can be parsed from various file formats.
    class BibleDataFile
    {
    public:
        // PARSING.
        static std::shared_ptr<BibleTranslation> ParseVersePerLineFile(const std::string& translation_name, const std::filesystem::path& filepath);
        static std::shared_ptr<BibleTranslation> ParseOsisXmlFile(const std::string& translation_name, const std::filesystem::path& filepath);
    };
}
