#pragma once

#include <memory>
#include <vector>
#include <ThirdParty/imgui/imgui.h>
#include "BibleData/Bible.h"
#include "BibleData/BibleVerse.h"
#include "BibleData/BibleVerseId.h"
#include "BibleData/BibleVerseRange.h"
#include "Gui/UserSettings.h"

namespace GUI
{
    /// A window for displaying Bible verses.
    class BibleVersesWindow
    {
    public:
        void UpdateAndRender(UserSettings& user_settings);

        void SetVerses(const BIBLE_DATA::BibleVerseRange& verse_range);

        /// True if the window is open; false otherwise.
        bool Open = false;
        /// The Bible from which to display verses.
        std::shared_ptr<BIBLE_DATA::Bible> Bible = nullptr;
        /// The range of verses displayed in the window.
        BIBLE_DATA::BibleVerseRange VerseRange = {};
        /// The verses to be rendered in the window by translation name.
        std::map<std::string, std::vector<BIBLE_DATA::BibleVerse>> VersesByTranslationName = {};

    private:
        void UpdateAndRenderVerseContent(const std::vector<BIBLE_DATA::BibleVerse>& verses, const std::map<std::string, ImVec4>& colors_by_word);
        void Render(const BIBLE_DATA::BibleVerse& verse);
        void UpdateColorLookup(std::map<std::string, ImVec4>& colors_by_word);
        void ComputeWordStatistics();

        /// The word currently being hovered over by the user.
        std::string CurrentlyHighlightedWord = "";
        
        /// True if the word panel is being displayed.
        bool DisplayingWordPanel = false;

        using WordAndCount = std::pair<std::string, unsigned int>;
        /// Counts for the number of times each word appears in the current window.
        std::vector<WordAndCount> WordsByDecreasingCount = {};
    };
}
