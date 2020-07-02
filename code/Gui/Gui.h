#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "BibleData/Bible.h"
#include "Gui/BibleBookWindow.h"
#include "Gui/BibleVersesWindow.h"
#include "Gui/Debugging/AboutWindow.h"
#include "Gui/Debugging/DemoWindow.h"
#include "Gui/Debugging/GuiMetricsWindow.h"
#include "Gui/Debugging/StyleEditorWindow.h"
#include "Gui/UserSettings.h"

/// Holds code for the main graphical user interface (GUI) of the program.
namespace GUI
{
    /// The root main graphical user interface of the program.
    /// This class acts as the centralized point for the main loop to call into.
    class Gui
    {
    public:
        void UpdateAndRender();


        /// The Bible being displayed in the GUI.
        std::shared_ptr<BIBLE_DATA::Bible> Bible = nullptr;

        /// The user's GUI settings.
        UserSettings UserSettings = {};

        /// A window for displaying and selecting Bible books.
        BibleBookWindow BibleBookWindow = {};
        /// A window for displaying Bible verses.
        BibleVersesWindow BibleVersesWindow = {};

        /// A window displaying basic GUI metrics.
        DEBUGGING::GuiMetricsWindow MetricsWindow = {};
        /// A window allowing editing of GUI styles.
        DEBUGGING::StyleEditorWindow StyleEditorWindow = {};
        /// A demo window for ImGui features.
        DEBUGGING::DemoWindow DemoWindow = {};
        /// A basic "about" window for the GUI.
        DEBUGGING::AboutWindow AboutWindow = {};
    };
}
