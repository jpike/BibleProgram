#pragma once

#include <future>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include "BibleData/Files/BibleDataFile.h"

/// Holds code related to raw files containing Bible data.
namespace BIBLE_DATA::FILES
{
    /// A collection of all Bible data files used by this program.
    /// This class encapsulates parsing data from multiple different files.
    class BibleDataFiles
    {
    public:
        // CONSTRUCTION.
        static BibleDataFiles StartLoading();
        explicit BibleDataFiles() = default;
        BibleDataFiles(BibleDataFiles&&) = default;
        BibleDataFiles& operator=(BibleDataFiles&&) = default;

        // FILE RETRIEVAL.
        std::optional<BibleDataFile> GetNextLoadedFile();

    private:
        /// Bible data files that are in the process of being loaded.
        std::vector<std::future<std::optional<BibleDataFile>>> FilesBeingLoaded = {};
    };
}
