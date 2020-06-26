#pragma once

#include <optional>
#include "BibleData/Bible.h"
#include "BibleData/BibleVerseRange.h"

namespace GUI
{
    /// A window that displays all books in the Bible and
    /// allows navigating through them by chapter.
    class BibleBookWindow
    {
    public:
        std::optional<BIBLE_DATA::BibleVerseRange> UpdateAndRender();

        /// True if the window is open; false otherwise.
        /// Defaults to true to give easy access to this window.
        bool Open = true;
    };
}
