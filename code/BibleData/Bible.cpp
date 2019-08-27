#include <unordered_map>
#include "BibleData/Bible.h"

namespace BIBLE_DATA
{
    /// Populates a Bible from the specified verses.
    /// @param[in]  verses - The verses in which to populate the Bible.
    /// @return The populated Bible.
    Bible Bible::Populate(const std::vector<BibleVerse>& verses)
    {
        Bible bible;

        // PLACE EACH VERSE IN THE APPROPRIATE LOCATION IN THE BIBLE.
        /// @todo   Make this code more robust to certain items not already existing.
        std::unordered_map<std::string, std::size_t> book_name_to_index_lookup;
        for (const auto& verse : verses)
        {
            // MAKE SURE THE VERSE'S BOOK EXISTS.
            bool book_already_exists = book_name_to_index_lookup.contains(verse.Book);
            if (!book_already_exists)
            {
                book_name_to_index_lookup[verse.Book] = bible.Books.size();
                bible.Books.push_back(BibleBook{ .Name = verse.Book });
            }

            // ADD THE VERSE TO THE BOOK.
            std::size_t current_book_index = book_name_to_index_lookup[verse.Book];
            auto& current_book = bible.Books[current_book_index];
            current_book.Add(verse);
        }

        return bible;
    }
}
