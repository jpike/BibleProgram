#pragma once

#include <map>
#include <string>
#include <ThirdParty/imgui/imgui.h>

namespace GUI
{
    /// Various user-specific settings for use across the GUI.
    class UserSettings
    {
    public:
        void SetPrimaryBibleTranslation(const std::string& translation_name);

        /// The primary Bible translation name (abbreviation) used
        /// for functionality that only operates on a single translation.
        std::string PrimaryBibleTranslationName = "";
        /// Bible translation display statuses (true = displayed; false = not displayed) in the window.
        std::map<std::string, bool> BibleTranslationDisplayStatusesByName = {};
        /// The colors associated with each word in the Bible.  Populated on-demand as needed.
        std::map<std::string, ImVec4> ColorsByWord = {};
    };
}
