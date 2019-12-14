#pragma once

#include <vector>
#include "BibleData/Bible.h"

/// Holds code for the main graphical user interface (GUI) of the program.
namespace GUI
{
    /// The root main graphical user interface of the program.
    /// This class acts as the centralized point for the main loop to call into.
    class Gui
    {
    public:
        void UpdateAndRender();
        void Render(const BIBLE_DATA::BibleVerse& verse);

        /// The Bible displayed in the GUI.
        BIBLE_DATA::Bible Bible = {};
    };
}
