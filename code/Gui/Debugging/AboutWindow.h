#pragma once

namespace GUI::DEBUGGING
{
    /// A window that display "about" information for the GUI.
    class AboutWindow
    {
    public:
        void UpdateAndRender();

        /// True if the window is open; false otherwise.
        bool Open = false;
    };
}
