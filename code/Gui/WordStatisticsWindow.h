#pragma once

#include <map>
#include <string>

namespace GUI
{
    /// A window displaying word statistics.
    class WordStatisticsWindow
    {
    public:
        void UpdateAndRender(std::map<std::string, unsigned int>& occurrence_counts_by_word);

        /// True if the window is open; false otherwise.
        bool Open = false;
    };
}
