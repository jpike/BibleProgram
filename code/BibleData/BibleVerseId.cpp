#include "BibleData/BibleVerseId.h"

namespace BIBLE_DATA
{
    bool BibleVerseId::operator==(const BibleVerseId& rhs) const
    {
        bool equal = (
            Book == rhs.Book &&
            ChapterNumber == rhs.ChapterNumber &&
            VerseNumber == rhs.VerseNumber);
        return equal;
    }

    bool BibleVerseId::operator!=(const BibleVerseId& rhs) const
    {
        bool equal = ((*this) == rhs);
        return !equal;
    }
    
    bool BibleVerseId::operator<(const BibleVerseId& rhs) const
    {
        if (Book < rhs.Book) return true;
        if (Book > rhs.Book) return false;

        // Books are equal...
        if (ChapterNumber < rhs.ChapterNumber) return true;
        if (ChapterNumber > rhs.ChapterNumber) return false;

        // Books and chapters are equal...
        bool this_verse_before_rhs_verse = (VerseNumber < rhs.VerseNumber);
        return this_verse_before_rhs_verse;
    }
}
