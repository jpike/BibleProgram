#include <algorithm>
#include <utility>
#include <vector>
#include <ThirdParty/imgui/imgui.h>
#include "Gui/WordStatisticsWindow.h"

namespace GUI
{
    /// Updates and renders the window if it's open.
    /// @param[in,out] occurrence_counts_by_word - The occurrence counts by word to display.
    void WordStatisticsWindow::UpdateAndRender(std::map<std::string, unsigned int>& occurrence_counts_by_word)
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        
        if (ImGui::Begin("Word Statistics", &Open))
        {
            /// @todo   Need to pre-compute this!  It tanks the frame rate!
            using WordAndCount = std::pair<std::string, unsigned int>;
            std::vector<WordAndCount> words_by_decreasing_count;
            for (const auto& word_and_count : occurrence_counts_by_word)
            {
                words_by_decreasing_count.emplace_back(
                    std::make_pair(word_and_count.first, word_and_count.second)
                );
            }

            std::sort(
                words_by_decreasing_count.begin(),
                words_by_decreasing_count.end(),
                [](const WordAndCount& left, const WordAndCount& right) -> bool
                {
                    return left.second > right.second;
                });

            for (const auto& word_and_count : words_by_decreasing_count)
            {
                std::size_t verse_count = word_and_count.second;
                std::string text = word_and_count.first + " = " + std::to_string(verse_count);
                ImGui::Text(text.c_str());
            }
        }
        ImGui::End();
    }
}
