#pragma once

#include <optional>
#include "BibleData/Bible.h"

namespace BIBLE_DATA
{
    /// A collection of all Bible data files used by this program.
    /// This class encapsulates parsing data from multiple different files.
    class BibleDataFiles
    {
    public:
        static std::optional<Bible> Parse();
    };
}
