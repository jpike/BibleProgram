#pragma once

#include <optional>
#include <filesystem>
#include <vector>
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    /// An XML file that contains Bible verses according the OSIS format: https://www.crosswire.org/osis/.
    class OsisXmlFile
    {
    public:
        // PARSING.
        static std::optional<OsisXmlFile> Parse(const std::filesystem::path& filepath);

        /// PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The verses parsed from the file.
        std::vector<BibleVerse> Verses = {};
    };
}
