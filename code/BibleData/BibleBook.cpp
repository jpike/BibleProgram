#include "BibleData/BibleBook.h"

namespace BIBLE_DATA
{
    /// Gets the book ID for the given string name or abbreviation.
    /// This method exists to centralize converting various string representations of books
    /// to a consistent standard representation for use within this program.
    /// @param[in]  book_name_or_abbrevation - The book name or abbreviation to get the ID for.
    /// @return BibleBookId::The book ID for the provided string representation of the book; may be INVALID
    ///     if the string book representation isn't recognized.
    BibleBookId BibleBook::GetId(const std::string& book_name_or_abbreviation)
    {
        // This code only looks for strings that have been needed to be recognized so far.
        // It may need to be updated if new data sources are supported.
        if (
            "Gen" == book_name_or_abbreviation ||
            "Genesis" == book_name_or_abbreviation)
        {
            return BibleBookId::GENESIS;
        }
        else if (
            "Exo" == book_name_or_abbreviation ||
            "Exod" == book_name_or_abbreviation ||
            "Exodus" == book_name_or_abbreviation)
        {
            return BibleBookId::EXODUS;
        }
        else if (
            "Lev" == book_name_or_abbreviation ||
            "Leviticus" == book_name_or_abbreviation)
        {
            return BibleBookId::LEVITICUS;
        }
        else if (
            "Num" == book_name_or_abbreviation ||
            "Numbers" == book_name_or_abbreviation)
        {
            return BibleBookId::NUMBERS;
        }
        else if (
            "Deu" == book_name_or_abbreviation ||
            "Deut" == book_name_or_abbreviation ||
            "Deuteronomy" == book_name_or_abbreviation)
        {
            return BibleBookId::DEUTERONOMY;
        }
        else if (
            "Jos" == book_name_or_abbreviation ||
            "Josh" == book_name_or_abbreviation ||
            "Joshua" == book_name_or_abbreviation)
        {
            return BibleBookId::JOSHUA;
        }
        else if (
            "Jdg" == book_name_or_abbreviation ||
            "Judg" == book_name_or_abbreviation ||
            "Judges" == book_name_or_abbreviation)
        {
            return BibleBookId::JUDGES;
        }
        else if (
            "Rut" == book_name_or_abbreviation ||
            "Ruth" == book_name_or_abbreviation)
        {
            return BibleBookId::RUTH;
        }
        else if (
            "Sa1" == book_name_or_abbreviation ||
            "1Sam" == book_name_or_abbreviation ||
            "1 Samuel" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_SAMUEL;
        }
        else if (
            "Sa2" == book_name_or_abbreviation ||
            "2Sam" == book_name_or_abbreviation ||
            "2 Samuel" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_SAMUEL;
        }
        else if (
            "Kg1" == book_name_or_abbreviation ||
            "1Kgs" == book_name_or_abbreviation ||
            "1 Kings" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_KINGS;
        }
        else if (
            "Kg2" == book_name_or_abbreviation ||
            "2Kgs" == book_name_or_abbreviation ||
            "2 Kings" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_KINGS;
        }
        else if (
            "Ch1" == book_name_or_abbreviation ||
            "1Chr" == book_name_or_abbreviation ||
            "1 Chronicles" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_CHRONICLES;
        }
        else if (
            "Ch2" == book_name_or_abbreviation ||
            "2Chr" == book_name_or_abbreviation ||
            "2 Chronicles" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_CHRONICLES;
        }
        else if (
            "Ezr" == book_name_or_abbreviation ||
            "Ezra" == book_name_or_abbreviation)
        {
            return BibleBookId::EZRA;
        }
        else if (
            "Neh" == book_name_or_abbreviation ||
            "Nehemiah" == book_name_or_abbreviation)
        {
            return BibleBookId::NEHEMIAH;
        }
        else if (
            "Est" == book_name_or_abbreviation ||
            "Esth" == book_name_or_abbreviation ||
            "Esther" == book_name_or_abbreviation)
        {
            return BibleBookId::ESTHER;
        }
        else if ("Job" == book_name_or_abbreviation)
        {
            return BibleBookId::JOB;
        }
        else if (
            "Psa" == book_name_or_abbreviation ||
            "Ps" == book_name_or_abbreviation ||
            "Psalm" == book_name_or_abbreviation ||
            "Psalms" == book_name_or_abbreviation)
        {
            return BibleBookId::PSALMS;
        }
        else if (
            "Pro" == book_name_or_abbreviation ||
            "Prov" == book_name_or_abbreviation ||
            "Proverbs" == book_name_or_abbreviation)
        {
            return BibleBookId::PROVERBS;
        }
        else if (
            "Ecc" == book_name_or_abbreviation ||
            "Eccl" == book_name_or_abbreviation ||
            "Ecclesiates" == book_name_or_abbreviation)
        {
            return BibleBookId::ECCLESIASTES;
        }
        else if (
            "Sol" == book_name_or_abbreviation ||
            "Song" == book_name_or_abbreviation ||
            "Song of Songs" == book_name_or_abbreviation ||
            "Song of Solomon" == book_name_or_abbreviation)
        {
            return BibleBookId::SONG_OF_SOLOMON;
        }
        else if (
            "Isa" == book_name_or_abbreviation ||
            "Isaiah" == book_name_or_abbreviation)
        {
            return BibleBookId::ISAIAH;
        }
        else if (
            "Jer" == book_name_or_abbreviation ||
            "Jeremiah" == book_name_or_abbreviation)
        {
            return BibleBookId::JEREMIAH;
        }
        else if (
            "Lam" == book_name_or_abbreviation ||
            "Lamentations" == book_name_or_abbreviation)
        {
            return BibleBookId::LAMENTATIONS;
        }
        else if (
            "Eze" == book_name_or_abbreviation ||
            "Ezek" == book_name_or_abbreviation ||
            "Ezekiel" == book_name_or_abbreviation)
        {
            return BibleBookId::EZEKIEL;
        }
        else if (
            "Dan" == book_name_or_abbreviation ||
            "Daniel" == book_name_or_abbreviation)
        {
            return BibleBookId::DANIEL;
        }
        else if (
            "Hos" == book_name_or_abbreviation ||
            "Hosea" == book_name_or_abbreviation)
        {
            return BibleBookId::HOSEA;
        }
        else if (
            "Joe" == book_name_or_abbreviation ||
            "Joel" == book_name_or_abbreviation)
        {
            return BibleBookId::JOEL;
        }
        else if (
            "Amo" == book_name_or_abbreviation ||
            "Amos" == book_name_or_abbreviation)
        {
            return BibleBookId::AMOS;
        }
        else if (
            "Oba" == book_name_or_abbreviation ||
            "Obad" == book_name_or_abbreviation ||
            "Obadiah" == book_name_or_abbreviation)
        {
            return BibleBookId::OBADIAH;
        }
        else if (
            "Jon" == book_name_or_abbreviation ||
            "Jonah" == book_name_or_abbreviation)
        {
            return BibleBookId::JONAH;
        }
        else if (
            "Mic" == book_name_or_abbreviation ||
            "Micah" == book_name_or_abbreviation)
        {
            return BibleBookId::MICAH;
        }
        else if (
            "Nah" == book_name_or_abbreviation ||
            "Nahum" == book_name_or_abbreviation)
        {
            return BibleBookId::NAHUM;
        }
        else if (
            "Hab" == book_name_or_abbreviation ||
            "Habakkuk" == book_name_or_abbreviation)
        {
            return BibleBookId::HABAKKUK;
        }
        else if (
            "Zep" == book_name_or_abbreviation ||
            "Zeph" == book_name_or_abbreviation ||
            "Zephaniah" == book_name_or_abbreviation)
        {
            return BibleBookId::ZEPHANIAH;
        }
        else if (
            "Hag" == book_name_or_abbreviation ||
            "Haggai" == book_name_or_abbreviation)
        {
            return BibleBookId::HAGGAI;
        }
        else if (
            "Zac" == book_name_or_abbreviation ||
            "Zech" == book_name_or_abbreviation ||
            "Zechariah" == book_name_or_abbreviation)
        {
            return BibleBookId::ZECHARIAH;
        }
        else if (
            "Mal" == book_name_or_abbreviation ||
            "Malachi" == book_name_or_abbreviation)
        {
            return BibleBookId::MALACHI;
        }
        else if (
            "Mat" == book_name_or_abbreviation ||
            "Matt" == book_name_or_abbreviation ||
            "Matthew" == book_name_or_abbreviation)
        {
            return BibleBookId::MATTHEW;
        }
        else if (
            "Mar" == book_name_or_abbreviation ||
            "Mark" == book_name_or_abbreviation)
        {
            return BibleBookId::MARK;
        }
        else if (
            "Luk" == book_name_or_abbreviation ||
            "Luke" == book_name_or_abbreviation)
        {
            return BibleBookId::LUKE;
        }
        else if (
            "Joh" == book_name_or_abbreviation ||
            "John" == book_name_or_abbreviation)
        {
            return BibleBookId::JOHN;
        }
        else if (
            "Act" == book_name_or_abbreviation ||
            "Acts" == book_name_or_abbreviation)
        {
            return BibleBookId::ACTS;
        }
        else if (
            "Rom" == book_name_or_abbreviation ||
            "Romans" == book_name_or_abbreviation)
        {
            return BibleBookId::ROMANS;
        }
        else if (
            "Co1" == book_name_or_abbreviation ||
            "1Cor" == book_name_or_abbreviation ||
            "1 Corinthians" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_CORINTHIANS;
        }
        else if (
            "Co2" == book_name_or_abbreviation ||
            "2Cor" == book_name_or_abbreviation ||
            "2 Corinthians" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_CORINTHIANS;
        }
        else if (
            "Gal" == book_name_or_abbreviation ||
            "Galatians" == book_name_or_abbreviation)
        {
            return BibleBookId::GALATIANS;
        }
        else if (
            "Eph" == book_name_or_abbreviation ||
            "Ephesians" == book_name_or_abbreviation)
        {
            return BibleBookId::EPHESIANS;
        }
        else if (
            "Phi" == book_name_or_abbreviation ||
            "Phil" == book_name_or_abbreviation ||
            "Philippians" == book_name_or_abbreviation)
        {
            return BibleBookId::PHILIPPIANS;
        }
        else if (
            "Col" == book_name_or_abbreviation ||
            "Colossians" == book_name_or_abbreviation)
        {
            return BibleBookId::COLOSSIANS;
        }
        else if (
            "Th1" == book_name_or_abbreviation ||
            "1Thess" == book_name_or_abbreviation ||
            "1 Thessalonians" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_THESSALONIANS;
        }
        else if (
            "Th2" == book_name_or_abbreviation ||
            "2Thess" == book_name_or_abbreviation ||
            "2 Thessalonians" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_THESSALONIANS;
        }
        else if (
            "Ti1" == book_name_or_abbreviation ||
            "1Tim" == book_name_or_abbreviation ||
            "1 Timothy" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_TIMOTHY;
        }
        else if (
            "Ti2" == book_name_or_abbreviation ||
            "2Tim" == book_name_or_abbreviation ||
            "2 Timothy" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_TIMOTHY;
        }
        else if (
            "Tit" == book_name_or_abbreviation ||
            "Titus" == book_name_or_abbreviation)
        {
            return BibleBookId::TITUS;
        }
        else if (
            "Plm" == book_name_or_abbreviation ||
            "Phlm" == book_name_or_abbreviation ||
            "Philemon" == book_name_or_abbreviation)
        {
            return BibleBookId::PHILEMON;
        }
        else if (
            "Heb" == book_name_or_abbreviation ||
            "Hebrews" == book_name_or_abbreviation)
        {
            return BibleBookId::HEBREWS;
        }
        else if (
            "Jam" == book_name_or_abbreviation ||
            "Jas" == book_name_or_abbreviation ||
            "James" == book_name_or_abbreviation)
        {
            return BibleBookId::JAMES;
        }
        else if (
            "Pe1" == book_name_or_abbreviation ||
            "1Pet" == book_name_or_abbreviation ||
            "1 Peter" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_PETER;
        }
        else if (
            "Pe2" == book_name_or_abbreviation ||
            "2Pet" == book_name_or_abbreviation ||
            "2 Peter" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_PETER;
        }
        else if (
            "Jo1" == book_name_or_abbreviation ||
            "1John" == book_name_or_abbreviation ||
            "1 John" == book_name_or_abbreviation)
        {
            return BibleBookId::FIRST_JOHN;
        }
        else if (
            "Jo2" == book_name_or_abbreviation ||
            "2John" == book_name_or_abbreviation ||
            "2 John" == book_name_or_abbreviation)
        {
            return BibleBookId::SECOND_JOHN;
        }
        else if (
            "Jo3" == book_name_or_abbreviation ||
            "3John" == book_name_or_abbreviation ||
            "3 John" == book_name_or_abbreviation)
        {
            return BibleBookId::THIRD_JOHN;
        }
        else if (
            "Jde" == book_name_or_abbreviation ||
            "Jude" == book_name_or_abbreviation)
        {
            return BibleBookId::JUDE;
        }
        else if (
            "Rev" == book_name_or_abbreviation ||
            "Revelation" == book_name_or_abbreviation)
        {
            return BibleBookId::REVELATION;
        }
        else
        {
            // Unrecognized strings can't be mapped to a known ID.
            return BibleBookId::INVALID;
        }
    }

    /// Gets the full name of the book given its ID.
    /// @param[in]  book_id - The ID of the book whose name to get.
    /// @return The full name of the book with the given ID.
    std::string BibleBook::FullName(const BibleBookId book_id)
    {
        // GET THE FULL NAME CORRESPONDING TO THE BOOK ID.
        switch (book_id)
        {
            case BibleBookId::GENESIS:
                return "Genesis";
            case BibleBookId::EXODUS:
                return "Exodus";
            case BibleBookId::LEVITICUS:
                return "Leviticus";
            case BibleBookId::NUMBERS:
                return "Numbers";
            case BibleBookId::DEUTERONOMY:
                return "Deuteronomy";
            case BibleBookId::JOSHUA:
                return "Joshua";
            case BibleBookId::JUDGES:
                return "Judges";
            case BibleBookId::RUTH:
                return "Ruth";
            case BibleBookId::FIRST_SAMUEL:
                return "1 Samuel";
            case BibleBookId::SECOND_SAMUEL:
                return "2 Samuel";
            case BibleBookId::FIRST_KINGS:
                return "1 Kings";
            case BibleBookId::SECOND_KINGS:
                return "2 Kings";
            case BibleBookId::FIRST_CHRONICLES:
                return "1 Chronicles";
            case BibleBookId::SECOND_CHRONICLES:
                return "2 Chronicles";
            case BibleBookId::EZRA:
                return "Ezra";
            case BibleBookId::NEHEMIAH:
                return "Nehemiah";
            case BibleBookId::ESTHER:
                return "Esther";
            case BibleBookId::JOB:
                return "Job";
            case BibleBookId::PSALMS:
                return "Psalms";
            case BibleBookId::PROVERBS:
                return "Proverbs";
            case BibleBookId::ECCLESIASTES:
                return "Ecclesiastes";
            case BibleBookId::SONG_OF_SOLOMON:
                return "Song of Solomon";
            case BibleBookId::ISAIAH:
                return "Isaiah";
            case BibleBookId::JEREMIAH:
                return "Jeremiah";
            case BibleBookId::LAMENTATIONS:
                return "Lamentations";
            case BibleBookId::EZEKIEL:
                return "Ezekiel";
            case BibleBookId::DANIEL:
                return "Daniel";
            case BibleBookId::HOSEA:
                return "Hosea";
            case BibleBookId::JOEL:
                return "Joel";
            case BibleBookId::AMOS:
                return "Amos";
            case BibleBookId::OBADIAH:
                return "Obadiah";
            case BibleBookId::JONAH:
                return "Jonah";
            case BibleBookId::MICAH:
                return "Micah";
            case BibleBookId::NAHUM:
                return "Nahum";
            case BibleBookId::HABAKKUK:
                return "Habakkuk";
            case BibleBookId::ZEPHANIAH:
                return "Zephaniah";
            case BibleBookId::HAGGAI:
                return "Haggai";
            case BibleBookId::ZECHARIAH:
                return "Zechariah";
            case BibleBookId::MALACHI:
                return "Malachi";
            case BibleBookId::MATTHEW:
                return "Matthew";
            case BibleBookId::MARK:
                return "Mark";
            case BibleBookId::LUKE:
                return "Luke";
            case BibleBookId::JOHN:
                return "John";
            case BibleBookId::ACTS:
                return "Acts";
            case BibleBookId::ROMANS:
                return "Romans";
            case BibleBookId::FIRST_CORINTHIANS:
                return "1 Corinthians";
            case BibleBookId::SECOND_CORINTHIANS:
                return "2 Corinthians";
            case BibleBookId::GALATIANS:
                return "Galatians";
            case BibleBookId::EPHESIANS:
                return "Ephesians";
            case BibleBookId::PHILIPPIANS:
                return "Philippians";
            case BibleBookId::COLOSSIANS:
                return "Colossians";
            case BibleBookId::FIRST_THESSALONIANS:
                return "1 Thessalonians";
            case BibleBookId::SECOND_THESSALONIANS:
                return "2 Thessalonians";
            case BibleBookId::FIRST_TIMOTHY:
                return "1 Timothy";
            case BibleBookId::SECOND_TIMOTHY:
                return "2 Timothy";
            case BibleBookId::TITUS:
                return "Titus";
            case BibleBookId::PHILEMON:
                return "Philemon";
            case BibleBookId::HEBREWS:
                return "Hebrews";
            case BibleBookId::JAMES:
                return "James";
            case BibleBookId::FIRST_PETER:
                return "1 Peter";
            case BibleBookId::SECOND_PETER:
                return "2 Peter";
            case BibleBookId::FIRST_JOHN:
                return "1 John";
            case BibleBookId::SECOND_JOHN:
                return "2 John";
            case BibleBookId::THIRD_JOHN:
                return "3 John";
            case BibleBookId::JUDE:
                return "Jude";
            case BibleBookId::REVELATION:
                return "Revelation";
            case BibleBookId::INVALID:
                // The invalid and default cases should be the same.
                [[fallthrough]];
            default:
                return "INVALID";
        }
    }
}
