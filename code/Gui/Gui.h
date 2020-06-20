#pragma once

#include <unordered_map>
#include <vector>
#include "BibleData/Bible.h"
#include "Gui/BibleBookWindow.h"
#include "Gui/BibleVersesWindow.h"
#include "Gui/Debugging/AboutWindow.h"
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
        /// A basic "about" window for the GUI.
        DEBUGGING::AboutWindow AboutWindow = {};

    private:
        void UpdateColorLookup(const BIBLE_DATA::Bible& bible);
    };

#if 0
    class DebugGui
    {
    public:
        void UpdateAndRenderMenu()
        {
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Metrics", nullptr, &MetricsWindowVisible);
                ImGui::MenuItem("Style Editor", nullptr, &StyleEditorVisible);
                ImGui::MenuItem("About", nullptr, &AboutWindowVisible);

                ImGui::EndMenu();
            }
        }

        void UpdateAndRender()
        {
            if (MetricsWindowVisible)
            {
                ImGui::ShowMetricsWindow(&MetricsWindowVisible);
            }

            if (StyleEditorVisible)
            {
                ImGui::Begin("Style Editor", &StyleEditorVisible);
                ImGui::ShowStyleEditor();
                ImGui::End();
            }

            if (AboutWindowVisible)
            {
                ImGui::ShowAboutWindow(&AboutWindowVisible);
            }
        }

        bool MetricsWindowVisible = false;
        bool StyleEditorVisible = false;
        bool AboutWindowVisible = false;
    };

    class BibleTranslationConfig
    {
    public:
        static BibleTranslationConfig Create(const BIBLE_DATA::Bible& bible)
        {
            BibleTranslationConfig translation_config;

            for (const auto& [translation_name, translation]: bible.TranslationsByName)
            {
                if (translation_config.CurrentTranslationName.empty())
                {
                    translation_config.CurrentTranslationName = translation_name;
                }

                translation_config.AvailableTranslationNames.push_back(translation_name);
                translation_config.EnabledStatusByTranslationName[translation_name] = true;
            }

            return translation_config;
        }

        std::string CurrentTranslationName = "";
        std::vector<std::string> AvailableTranslationNames = {};
        std::unordered_map<std::string, bool> EnabledStatusByTranslationName = {};
    };

    /// The root main graphical user interface of the program.
    /// This class acts as the centralized point for the main loop to call into.
    class Gui
    {
    public:
        void UpdateAndRender();
        void Render(BIBLE_DATA::BibleVerse& verse);

        DebugGui DebugGui = {};

        BibleTranslationConfig TranslationConfig = {};

        const BIBLE_DATA::BibleChapter* SelectedChapter = nullptr;

        /// The Bible displayed in the GUI.
        BIBLE_DATA::Bible Bible = {};

        std::unordered_map<
            std::string,
            std::vector<BIBLE_DATA::BibleVerse>> CurrentVersesByTranslationName = {};
    };
#endif
}
