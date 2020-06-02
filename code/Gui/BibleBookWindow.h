#pragma once

#include "BibleData/Bible.h"

namespace GUI
{
    /// A window that displays all books in the Bible and
    /// allows navigating through them by chapter.
    class BibleBookWindow
    {
    public:
        const BIBLE_DATA::BibleChapter* UpdateAndRender(const BIBLE_DATA::Bible& bible);

        /// True if the window is open; false otherwise.
        bool Open = false;
    };
}
