#include "Gui/UserSettings.h"

namespace GUI
{
    /// Sets the primary Bible translation in the user settings, updating all relevant settings.
    /// @param[in]  translation_name - The name (abbreviation) of the primary translation.
    void UserSettings::SetPrimaryBibleTranslation(const std::string& translation_name)
    {
        // SET THE PRIMARY TRANSLATION.
        PrimaryBibleTranslationName = translation_name;

        // ENSURE THE PRIMARY BIBLE TRANSLATION IS DISPLAYED BY DEFAULT.
        BibleTranslationDisplayStatusesByName[translation_name] = true;
    }
}
