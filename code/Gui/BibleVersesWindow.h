#pragma once

#include <vector>
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"

namespace GUI
{
    /// A window for displaying Bible verses.
    class BibleVersesWindow
    {
    public:
        void UpdateAndRender();

        /// True if the window is open; false otherwise.
        bool Open = false;
        /// The ID of the first verse displayed in the window.
        BIBLE_DATA::BibleVerseId StartingVerseId = {};
        /// The ID of the last verse displayed in the window.
        BIBLE_DATA::BibleVerseId EndingVerseId = {};
        /// The verses to be rendered in the window.
        std::vector<BIBLE_DATA::BibleVerse> Verses = {};

    private:
        void Render(const BIBLE_DATA::BibleVerse& verse);
    };
}
