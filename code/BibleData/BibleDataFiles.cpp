#include <future>
#include "BibleData/BibleDataFiles.h"
#include "BibleData/OsisXmlFile.h"
#include "BibleData/VersePerLineFile.h"

namespace BIBLE_DATA
{
    /// Attempts to parse all Bible data files into a Bible.
    /// @return A Bible populated from the data files, if successfully parsed; std::nullopt otherwise.
    std::optional<Bible> BibleDataFiles::Parse()
    {
        // PARSE ALL OF THE DATA FILES.
        // They're parsed in separate threads to speed up loading.
        /// \todo   Re-evaluate this to see if it's actually needed.
        std::future<std::optional<VersePerLineFile>> kjv_bible_loading = std::async(std::launch::async, [] { return VersePerLineFile::Parse("data/SacredTexts/kjvdat.txt"); });
        std::future<std::optional<OsisXmlFile>> bbe_bible_loading = std::async(std::launch::async, [] { return OsisXmlFile::Parse("data/GratisBible/bbe.xml"); });
        std::future<std::optional<OsisXmlFile>> web_bible_loading = std::async(std::launch::async, [] { return OsisXmlFile::Parse("data/GratisBible/web.xml"); });
        std::future<std::optional<OsisXmlFile>> ylt_bible_loading = std::async(std::launch::async, [] { return OsisXmlFile::Parse("data/GratisBible/ylt.xml"); });

        std::optional<VersePerLineFile> kjv_bible_file = kjv_bible_loading.get();
        std::optional<OsisXmlFile> bbe_bible_file = bbe_bible_loading.get();
        std::optional<OsisXmlFile> web_bible_file = web_bible_loading.get();
        std::optional<OsisXmlFile> ylt_bible_file = ylt_bible_loading.get();
        bool all_bibles_parsed = (
            kjv_bible_file &&
            bbe_bible_file &&
            web_bible_file &&
            ylt_bible_file);
        if (!all_bibles_parsed)
        {
            // INDICATE THAT PARSING FAILED.
            return std::nullopt;
        }

        // POPULATE THE BIBLE WITH ALL TRANSLATIONS.
        Bible bible;
        bible.AddTranslation("BBE", bbe_bible_file->Verses, bbe_bible_file->BooksById);
        bible.AddTranslation("KJV", kjv_bible_file->Verses, kjv_bible_file->BooksById);
        bible.AddTranslation("WEB", web_bible_file->Verses, web_bible_file->BooksById);
        bible.AddTranslation("YLT", ylt_bible_file->Verses, ylt_bible_file->BooksById);
        return bible;
    }
}
