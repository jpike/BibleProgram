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
        std::optional<VersePerLineFile> kjv_bible_file = VersePerLineFile::Parse("data/SacredTexts/kjvdat.txt");
        std::optional<OsisXmlFile> bbe_bible_file = OsisXmlFile::Parse("data/GratisBible/bbe.xml");
        std::optional<OsisXmlFile> web_bible_file = OsisXmlFile::Parse("data/GratisBible/web.xml");
        std::optional<OsisXmlFile> ylt_bible_file = OsisXmlFile::Parse("data/GratisBible/ylt.xml");
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
        bible.AddTranslation("BBE", bbe_bible_file->Verses);
        bible.AddTranslation("KJV", kjv_bible_file->Verses);
        bible.AddTranslation("WEB", web_bible_file->Verses);
        bible.AddTranslation("YLT", ylt_bible_file->Verses);
        return bible;
    }
}
