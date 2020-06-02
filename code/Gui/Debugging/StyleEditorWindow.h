#pragma once

namespace GUI::DEBUGGING
{
    /// A window that allows tweaking the style of the GUI.
    class StyleEditorWindow
    {
    public:
        void UpdateAndRender();

        /// True if the window is open; false otherwise.
        bool Open = false;
    };
}
