#pragma once

#include <unordered_map>
#include <vector>
#include "BibleData/Bible.h"
#include "Gui/BibleBookWindow.h"
#include "Gui/BibleVersesWindow.h"
#include "Gui/Debugging/AboutWindow.h"
#include "Gui/Debugging/DemoWindow.h"
#include "Gui/Debugging/GuiMetricsWindow.h"
#include "Gui/Debugging/StyleEditorWindow.h"
#include "Gui/WordColorsWindow.h"
#include "Gui/WordStatisticsWindow.h"

/// Holds code for the main graphical user interface (GUI) of the program.
namespace GUI
{
    /// The root main graphical user interface of the program.
    /// This class acts as the centralized point for the main loop to call into.
    class Gui
    {
    public:
        void UpdateAndRender(const BIBLE_DATA::Bible& bible);


        /// The name of the currently selected translation.
        std::string CurrentTranslationName = "";

        /// The colors associated with each word.  Populated on-demand as needed.
        std::map<std::string, ImVec4> ColorsByWord = {};

        /// A window for displaying and selecting Bible books.
        BibleBookWindow BibleBookWindow = {};
        /// A window for displaying Bible verses.
        BibleVersesWindow BibleVersesWindow = {};
        /// A window for displaying colors for words.
        WordColorsWindow WordColorsWindow = {};
        /// A window for displaying statistics about words.
        WordStatisticsWindow WordStatisticsWindow = {};

        /// A window displaying basic GUI metrics.
        DEBUGGING::GuiMetricsWindow MetricsWindow = {};
        /// A window allowing editing of GUI styles.
        DEBUGGING::StyleEditorWindow StyleEditorWindow = {};
        /// A demo window for ImGui features.
        DEBUGGING::DemoWindow DemoWindow = {};
        /// A basic "about" window for the GUI.
        DEBUGGING::AboutWindow AboutWindow = {};

    private:
        void UpdateColorLookup(const BIBLE_DATA::Bible& bible);
    };
}
