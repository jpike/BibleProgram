#pragma once

#include <vector>
#include <ThirdParty/imgui/imgui.h>
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
        void UpdateAndRenderVerseContent(const std::vector<BIBLE_DATA::BibleVerse>& verses);
        void Render(const BIBLE_DATA::BibleVerse& verse);
        void UpdateColorLookup();
        void ComputeWordStatistics();

        /// The word currently being hovered over by the user.
        std::string CurrentlyHighlightedWord = "";
        /// The colors associated with each word.  Populated on-demand as needed.
        std::map<std::string, ImVec4> ColorsByWord = {};
        /// Counts for the number of times each word appears in the current window.
        std::map<std::string, unsigned int> OccurrenceCountsByWord = {};
    };
}