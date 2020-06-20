#pragma once

#include <map>
#include <string>
#include <ThirdParty/imgui/imgui.h>

namespace GUI
{
    /// A window displaying word colors.
    class WordColorsWindow
    {
    public:
        void UpdateAndRender(std::map<std::string, ImVec4>& colors_by_word);

        /// True if the window is open; false otherwise.
        bool Open = false;
    };
}
