#pragma once

#include <unordered_map>
#include <vector>
#include "BibleData/Bible.h"

/// Holds code for the main graphical user interface (GUI) of the program.
namespace GUI
{
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
}
