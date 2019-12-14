#pragma once

#include <string>
#include "BibleData/BibleVerseId.h"

/// Holds code related to accessing and manipulating underlying data about the Bible.
namespace BIBLE_DATA
{
    /// A single verse from the Bible.
    class BibleVerse
    {
    public:
        /// The ID of the verse.
        BibleVerseId Id = {};
        /// The text of the verse.
        std::string Text = "";
    };
}
