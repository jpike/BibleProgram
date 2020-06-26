#pragma once

namespace GUI::DEBUGGING
{
    /// A window for demoing ImGui features.
    class DemoWindow
    {
    public:
        void UpdateAndRender();

        /// True if the window is open; false otherwise.
        bool Open = false;
    };
}
