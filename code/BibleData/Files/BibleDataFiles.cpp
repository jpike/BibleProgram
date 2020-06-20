#include "BibleData/Files/BibleDataFiles.h"

namespace BIBLE_DATA::FILES
{
    /// Starts loading all Bible data files.
    /// @return The Bible data files that are in the process of being loaded.
    BibleDataFiles BibleDataFiles::StartLoading()
    {
        // START PARSING ALL OF THE DATA FILES.
        // They're parsed in separate threads to speed up loading.
        /// \todo   Re-evaluate this to see if it's actually needed.
        std::future<std::optional<BibleDataFile>> kjv_bible_loading = std::async(std::launch::async, [] { return BibleDataFile::ParseVersePerLineFile("KJV", "data/SacredTexts/kjvdat.txt"); });
#if 0
        std::future<std::optional<BibleDataFile>> bbe_bible_loading = std::async(std::launch::async, [] { return BibleDataFile::ParseOsisXmlFile("BBE", "data/GratisBible/bbe.xml"); });
        std::future<std::optional<BibleDataFile>> web_bible_loading = std::async(std::launch::async, [] { return BibleDataFile::ParseOsisXmlFile("WEB", "data/GratisBible/web.xml"); });
        std::future<std::optional<BibleDataFile>> ylt_bible_loading = std::async(std::launch::async, [] { return BibleDataFile::ParseOsisXmlFile("YLT", "data/GratisBible/ylt.xml"); });
#endif

        // STORE THE FILES BEING LOADED.
        BibleDataFiles bible_data_files;
        bible_data_files.FilesBeingLoaded.emplace_back(std::move(kjv_bible_loading));
#if 0
        bible_data_files.FilesBeingLoaded.emplace_back(std::move(bbe_bible_loading));
        bible_data_files.FilesBeingLoaded.emplace_back(std::move(web_bible_loading));
        bible_data_files.FilesBeingLoaded.emplace_back(std::move(ylt_bible_loading));       
#endif
        return std::move(bible_data_files);
    }

    /// Gets the next loaded Bible data file, if one exists.
    /// Any returned file is removed from this collection.
    /// @return The next loaded Bible data file, if one exists.
    std::optional<BibleDataFile> BibleDataFiles::GetNextLoadedFile()
    {
        // FIND THE NEXT AVAILABLE FILE.
        std::optional<BibleDataFile> next_bible_data_file;
        for (auto file_being_loaded = FilesBeingLoaded.begin();
            file_being_loaded != FilesBeingLoaded.end();)
        {
            // CHECK IF THE CURRENT FILE IS AVAILABLE.
            bool current_file_available = file_being_loaded->valid();
            if (current_file_available)
            {
                // TRACK THE FILE IF IT'S LOADED.
                next_bible_data_file = file_being_loaded->get();
                if (next_bible_data_file)
                {
                    // If the file is loaded, then we can remove it from the queue
                    // to reduce future searching.
                    file_being_loaded = FilesBeingLoaded.erase(file_being_loaded);

                    // There's no need to continue searching if a valid file was found.
                    break;
                }
                else
                {
                    // ERASE THE FILE SINCE IT FAILED TO LOAD.
                    file_being_loaded = FilesBeingLoaded.erase(file_being_loaded);
                }
            }
            else
            {
                // MOVE TO CHECKING THE NEXT FILE.
                ++file_being_loaded;
            }
        }

        return next_bible_data_file;
    }
}
