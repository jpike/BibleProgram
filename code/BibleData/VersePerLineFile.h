#pragma once

#include <optional>
#include <filesystem>
#include <vector>
#include "BibleData/BibleVerse.h"

namespace BIBLE_DATA
{
    class VersePerLineFile
    {
    public:
        static std::optional<VersePerLineFile> Parse(const std::filesystem::path& filepath);

        std::vector<BibleVerse> Verses = {};
    };
}
