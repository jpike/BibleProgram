#pragma once

/// Holds GUI components to assist in debugging.
namespace GUI::DEBUGGING
{
    /// A window that displays various GUI metrics.
    class GuiMetricsWindow
    {
    public:
        void UpdateAndRender();

        /// True if the window is open; false otherwise.
        bool Open = false;
    };
}
