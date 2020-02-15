#include <ThirdParty/pugixml/pugixml.hpp>
#include "BibleData/BibleBook.h"
#include "BibleData/OsisXmlFile.h"

namespace BIBLE_DATA
{
    /// Attempts to parse the file at the specified path.
    /// @param[in]  filepath - The path to the file to parse.
    /// @return The parsed verse-per-line file, if successful; std::nullopt otherwise.
    std::optional<OsisXmlFile> OsisXmlFile::Parse(const std::filesystem::path& filepath)
    {
        // PARSE THE XML FILE.
        pugi::xml_document osis_xml_document;
        pugi::xml_parse_result parse_result = osis_xml_document.load_file(filepath.c_str());
        if (!parse_result)
        {
            return std::nullopt;
        }

        // PARSE VERSES FROM EACH BOOK IN THE FILE.
        OsisXmlFile parsed_bible;
        std::vector<BibleVerse> verses;
        pugi::xml_node osis_text_xml_node = osis_xml_document.select_node("/osis/osisText").node();
        // An assumption is being made that all direct child divs are for books, which is consistent with all files thus far.
        auto book_xml_nodes = osis_text_xml_node.children("div");
        for (const pugi::xml_node& book_xml_node : book_xml_nodes)
        {
            // PARSE ALL CHAPTERS FROM THE CURRENT BOOK.
            auto chapter_xml_nodes = book_xml_node.children("chapter");
            for (const pugi::xml_node& chapter_xml_node : chapter_xml_nodes)
            {
                // PARSE ALL VERSES FROM THE CURRENT CHAPTER.
                auto verse_xml_nodes = chapter_xml_node.children("verse");
                for (const pugi::xml_node& verse_xml_node : verse_xml_nodes)
                {
                    // PARSE IDENTIFYING INFORMATION ABOUT THE VERSE.
                    pugi::xml_attribute verse_id_attribute = verse_xml_node.attribute("osisID");
                    std::string verse_id = verse_id_attribute.as_string();

                    /// @todo   Factor this out into helper function!
                    std::vector<std::string> verse_id_components;
                    const char VERSE_ID_COMPONENT_DELIMITER = '.';
                    std::size_t string_search_start_index = 0;
                    std::size_t next_verse_id_component_delimiter_index = verse_id.find(VERSE_ID_COMPONENT_DELIMITER, string_search_start_index);
                    while (std::string::npos != next_verse_id_component_delimiter_index)
                    {
                        // STORE THE CURRENT COMPONENT.
                        std::size_t current_component_length_in_characters = next_verse_id_component_delimiter_index - string_search_start_index;
                        std::string verse_id_component = verse_id.substr(string_search_start_index, current_component_length_in_characters);
                        verse_id_components.push_back(verse_id_component);

                        // CONTINUE SEARCHING FOR MORE COMPONENTS.
                        string_search_start_index = next_verse_id_component_delimiter_index + 1;
                        next_verse_id_component_delimiter_index = verse_id.find(VERSE_ID_COMPONENT_DELIMITER, string_search_start_index);
                    }

                    // The final component won't be parsed in the last iteration of the loop.
                    std::size_t verse_number_length_in_characters = next_verse_id_component_delimiter_index - string_search_start_index;
                    std::string verse_number_component = verse_id.substr(string_search_start_index, verse_number_length_in_characters);
                    verse_id_components.push_back(verse_number_component);

                    const std::size_t BOOK_COMPONENT_INDEX = 0;
                    std::string book_name = verse_id_components.at(BOOK_COMPONENT_INDEX);
                    BibleBookId book_id = BibleBook::GetId(book_name);

                    const std::size_t CHAPTER_COMPONENT_INDEX = 1;
                    std::string chapter_number_string = verse_id_components.at(CHAPTER_COMPONENT_INDEX);
                    unsigned int chapter_number = std::stoi(chapter_number_string);

                    const std::size_t VERSE_NUMBER_COMPONENT_INDEX = 2;
                    std::string verse_number_string = verse_id_components.at(VERSE_NUMBER_COMPONENT_INDEX);
                    unsigned int verse_number = std::stoi(verse_number_string);

                    // GET THE VERSE TEXT.
                    std::string verse_text = verse_xml_node.text().as_string();

                    // The tokenized form is needed as well.
                    /// \todo std::vector<Token> verse_tokens = BibleVerse::Tokenize(verse_text);

                    // STORE THE PARSED VERSE.
                    BibleVerse verse
                    {
                        .Id = BibleVerseId {.Book = book_id, .ChapterNumber = chapter_number, .VerseNumber = verse_number },
                        .Text = verse_text,
                        /// \todo .Tokens = verse_tokens
                    };
                    parsed_bible.Verses.push_back(verse);

                    // PLACE THE VERSE IN ITS BOOK.
                    parsed_bible.BooksById[book_id].Id = book_id;
                    parsed_bible.BooksById[book_id].AddVerse(verse);
                }
            }
        }

        // RETURN THE PARSED FILE.
        return parsed_bible;
    }
}
