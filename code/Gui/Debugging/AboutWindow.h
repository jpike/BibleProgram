#pragma once

namespace GUI::DEBUGGING
{
    /// A window that displays "about" information for the program.
    class AboutWindow
    {
    public:
        void UpdateAndRender();

        /// True if the window is open; false otherwise.
        bool Open = false;
        /// True if the "about GUI" sub-window is open; false otherwise.
        bool GuiSubWindowOpen = false;
    };
}
