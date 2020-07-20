#include <algorithm>
#include <cctype>
#include <random>
#include <unordered_set>
#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_internal.h>
#include "BibleData/BibleBook.h"
#include "Gui/BibleVersesWindow.h"

namespace GUI
{
    /// Updates and renders a single frame of the window, if it's open.
    /// @param[in,out]  user_settings - The user settings for the GUI.
    void BibleVersesWindow::UpdateAndRender(UserSettings& user_settings)
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        // RENDER EACH VERSE IN THE WINDOW.
        std::string window_title = (
            BIBLE_DATA::BibleBook::FullName(VerseRange.StartingVerse.Book) + " " +
            std::to_string(VerseRange.StartingVerse.ChapterNumber) + ":" +
            std::to_string(VerseRange.StartingVerse.VerseNumber));
        bool multiple_verses = (VerseRange.StartingVerse != VerseRange.EndingVerse);
        if (multiple_verses)
        {
            window_title += " - " +
                BIBLE_DATA::BibleBook::FullName(VerseRange.EndingVerse.Book) + " " +
                std::to_string(VerseRange.EndingVerse.ChapterNumber) + ":" +
                std::to_string(VerseRange.EndingVerse.VerseNumber);
        }
        std::string window_title_and_id = window_title + "###Verses";

        // Window positioning/sizing is only done upon the first use to allow preserving a user's manual changes.
        // The window is positioned to basically be to the right of the Bible book window.
        ImVec2 current_drawing_cursor_position = ImGui::GetCursorPos();
        constexpr float BOOK_WINDOW_WIDTH_IN_PIXELS = 200.0f;
        constexpr float ADDITIONAL_PADDING_IN_PIXELS = 4.0f;
        float verses_window_default_x_position = current_drawing_cursor_position.x + BOOK_WINDOW_WIDTH_IN_PIXELS + ADDITIONAL_PADDING_IN_PIXELS;
        ImVec2 verses_window_default_position(verses_window_default_x_position, current_drawing_cursor_position.y);
        ImGui::SetNextWindowPos(verses_window_default_position, ImGuiCond_FirstUseEver);
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 available_screen_space_in_pixels = io.DisplaySize - verses_window_default_position;
        ImGui::SetNextWindowSize(available_screen_space_in_pixels, ImGuiCond_FirstUseEver);

        if (ImGui::Begin(window_title_and_id.c_str(), &Open, ImGuiWindowFlags_MenuBar))
        {
            // RENDER A MENU BAR.
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Translations"))
                {
                    for (const auto& name_and_translation : Bible->TranslationsByName)
                    {
                        // No shortcut keys are displayed for menu items since ImGui doesn't currently handle them.
                        constexpr char* NO_SHORTCUT_KEYS = nullptr;

                        bool translation_displayed = user_settings.BibleTranslationDisplayStatusesByName[name_and_translation.first];
                        if (ImGui::MenuItem(name_and_translation.first.c_str(), NO_SHORTCUT_KEYS, translation_displayed))
                        {
                            user_settings.BibleTranslationDisplayStatusesByName[name_and_translation.first] = !translation_displayed;

                            SetVerses(VerseRange);
                        };
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::Button("Words"))
                {
                    DisplayingWordPanel = !DisplayingWordPanel;

                    if (WordsByDecreasingCount.empty())
                    {
                        ComputeWordStatistics();
                    }
                }

                ImGui::EndMenuBar();
            }

            // DETERMINE THE NUMBER OF COLUMNS TO DISPLAY IN THE WINDOW.
            // One column is needed per displayed translation.
            unsigned int column_count = 0;
            for (const auto& name_and_translation : Bible->TranslationsByName)
            {
                bool translation_displayed = user_settings.BibleTranslationDisplayStatusesByName[name_and_translation.first];
                if (translation_displayed)
                {
                    ++column_count;
                }
            }

            // The word panel requires an additional column.
            if (DisplayingWordPanel)
            {
                ++column_count;
            }

            // CHECK IF ANY COLUMNS EXIST.
            // If data is still being loaded, then no columns will exist.
            // Providing 0 columns to ImGui will cause a crash.
            bool columns_exist = (column_count > 0);
            if (columns_exist)
            {
                // SET THE COLUMNS FOR THIS WINDOW.
                ImGui::Columns(column_count);

                // RENDER A COLUMN FOR EACH VISIBLE TRANSLATION.
                for (const auto& name_and_translation : Bible->TranslationsByName)
                {
                    // CHECK IF THE CURRENT TRANSLATION IS DISPLAYED.
                    bool translation_displayed = user_settings.BibleTranslationDisplayStatusesByName[name_and_translation.first];
                    if (translation_displayed)
                    {
                        // RENDER A CHILD WINDOW FOR THE TRANSLATION.
                        // This allows for independent scrolling of each column.
                        if (ImGui::BeginChild(name_and_translation.first.c_str()))
                        {
                            // RENDER THE TRANSLATION NAME.
                            ImGui::Text(name_and_translation.first.c_str());

                            // REDNER THE VERSES FOR THE TRANSLATION.
                            const auto& verses_for_translation = VersesByTranslationName[name_and_translation.first];
                            UpdateAndRenderVerseContent(verses_for_translation, user_settings.ColorsByWord);
                        }
                        ImGui::EndChild();

                        // MOVE TO THE NEXT COLUMN.
                        ImGui::NextColumn();
                    }
                }

                // RENDER THE WORD PANEL IF IT'S VISIBLE
                if (DisplayingWordPanel)
                {
                    if (ImGui::BeginChild("###Words"))
                    {
                        // ADD BUTTONS FOR COLORING.
                        if (ImGui::Button("Auto-Colorize"))
                        {
                            UpdateColorLookup(user_settings.ColorsByWord);
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Clear Colors"))
                        {
                            user_settings.ColorsByWord.clear();
                        }

                        for (const auto& word_and_count : WordsByDecreasingCount)
                        {
                            std::size_t verse_count = word_and_count.second;
                            std::string word_and_count_text = word_and_count.first + " = " + std::to_string(verse_count);
                            
                            ImVec4 word_color = { 1.0f, 1.0f, 1.0f, 1.0f };
#if __EMSCRIPTEN__
                            unsigned int color_count_for_word = user_settings.ColorsByWord.count(word_and_count.first);
                            bool word_already_has_color = (color_count_for_word > 0);
#else
                            bool word_already_has_color = user_settings.ColorsByWord.contains(word_and_count.first);
#endif
                            if (word_already_has_color)
                            {
                                word_color = user_settings.ColorsByWord[word_and_count.first];
                            }
                            float color_components[3] =
                            {
                                word_color.x,
                                word_color.y,
                                word_color.z,
                            };

                            // No label is displayed because it results in too much space to the right of the colored square.
                            if (ImGui::ColorEdit3(
                                word_and_count_text.c_str(),
                                color_components,
                                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                            {
                                word_color.x = color_components[0];
                                word_color.y = color_components[1];
                                word_color.z = color_components[2];
                                user_settings.ColorsByWord[word_and_count.first] = word_color;
                            }

                            ImGui::SameLine();
                            ImGui::TextColored(word_color, word_and_count_text.c_str());
                        }

#if TODO_HAVE_WAYS_TO_REORDER
                        for (auto& word_and_color : user_settings.ColorsByWord)
                        {
                            float color_components[3] =
                            {
                                word_and_color.second.x,
                                word_and_color.second.y,
                                word_and_color.second.z,
                            };

                            ImGui::ColorEdit3(
                                word_and_color.first.c_str(),
                                color_components,
                                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                            word_and_color.second.x = color_components[0];
                            word_and_color.second.y = color_components[1];
                            word_and_color.second.z = color_components[2];

                            ImGui::SameLine();
                            ImGui::Text(word_and_color.first.c_str());
                        }

                        for (const auto& word_and_count : WordsByDecreasingCount)
                        {
                            std::size_t verse_count = word_and_count.second;
                            std::string text = word_and_count.first + " = " + std::to_string(verse_count);
                            ImGui::Text(text.c_str());
                        }
#endif
                    }
                    ImGui::EndChild();
                }

                // RETURN TO THE NORMAL NUMBER OF COLUMNS.
                ImGui::Columns();
            }
        }
        ImGui::End();
    }

    /// Sets the verses displayed in the window.
    /// @param[in]  verse_range - The range of verses.
    void BibleVersesWindow::SetVerses(const BIBLE_DATA::BibleVerseRange& verse_range)
    {
        VerseRange = verse_range;
        
        for (const auto& name_and_translation : Bible->TranslationsByName)
        {
            std::vector<BIBLE_DATA::BibleVerse> verses_for_translation = name_and_translation.second->GetVerses(VerseRange);
            VersesByTranslationName[name_and_translation.first] = verses_for_translation;
        }

        WordsByDecreasingCount.clear();
        if (DisplayingWordPanel)
        {
            ComputeWordStatistics();
        }
    }

    struct TextRenderCommand
    {
        std::string Text = "";
        ImVec4 Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec2 DrawPosition = ImVec2(0.0f, 0.0f);
        ImVec2 TextSize = ImVec2(0.0f, 0.0f);
        ImRect TextBoundingBox = ImRect(0.0f, 0.0f, 0.0f, 0.0f);
    };

    /// Updates and renders the specified verse content in the window.
    /// Some of the implementation here is based on ImGui's multi-line text input
    /// but modified/simplified for our purposes here.
    /// @param[in]  verses - The verse content to render.
    /// @param[in]  colors_by_word - Colors for words.
    void BibleVersesWindow::UpdateAndRenderVerseContent(const std::vector<BIBLE_DATA::BibleVerse>& verses, const std::map<std::string, ImVec4>& colors_by_word)
    {
        // COMPUTE THE BOUNDING BOX FOR THE VERSE TEXT.
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_content_region_size = ImGui::GetWindowContentRegionMax();
        ImVec2 entire_text_bounding_box_max_coordinates = window->DC.CursorPos + window_content_region_size;
        // Each window has some amount of padding (indentation) around its border for readability
        // that needs to be accounted for in order to keep the right side of the next from
        // bumping up against the left of the window.
        entire_text_bounding_box_max_coordinates.x -= window->DC.Indent.x;
        ImRect entire_text_bounding_box(window->DC.CursorPos, entire_text_bounding_box_max_coordinates);

        // FORM THE FINAL TEXT FOR THE VERSES.
        std::vector<TextRenderCommand> text_render_commands;

        BIBLE_DATA::BibleBookId previous_book = BIBLE_DATA::BibleBookId::INVALID;
        unsigned int previous_chapter_number = 0;
        ImVec2 current_draw_position = window->DC.CursorPos;
        bool first_verse = true;
        bool first_chapter = true;
        bool first_book = true;
        for (const BIBLE_DATA::BibleVerse& verse : verses)
        {
            // PRINT SOME TEXT TO INDICATE IF A NEW BOOK IS BEING STARTED.
            bool new_book_being_started = (previous_book != verse.Id.Book);
            if (new_book_being_started)
            {
                if (!first_book)
                {
                    current_draw_position.y += ImGui::GetTextLineHeight();
                    text_render_commands.back().Text += '\n';
                }
                current_draw_position.x = entire_text_bounding_box.GetBL().x;

                std::string book_name = BIBLE_DATA::BibleBook::FullName(verse.Id.Book);
                std::transform(
                    book_name.begin(),
                    book_name.end(),
                    book_name.begin(),
                    [](const char character) { return static_cast<char>(std::toupper(character)); });

                ImVec2 text_size = ImGui::CalcTextSize(book_name.c_str());

                TextRenderCommand book_title_render_command =
                {
                    .Text = book_name + '\n',
                    //.Text = book_name,
                    .Color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    .DrawPosition = current_draw_position,
                };
                book_title_render_command.TextSize = ImGui::CalcTextSize(book_title_render_command.Text.c_str());
                book_title_render_command.TextBoundingBox = ImRect(
                    book_title_render_command.DrawPosition, 
                    book_title_render_command.DrawPosition + text_size);
                text_render_commands.push_back(book_title_render_command);

                //current_draw_position.x += text_size.x;
                current_draw_position.y += ImGui::GetTextLineHeight();

                previous_book = verse.Id.Book;
                first_book = false;
                first_chapter = true;
            }
            // PRINT SOME TEXT TO INDICATE IF A NEW CHAPTER IS BEING STARTED.
            bool new_chapter_being_started = (previous_chapter_number != verse.Id.ChapterNumber);
            if (new_chapter_being_started)
            {
                if (!first_chapter)
                {
                    current_draw_position.y += ImGui::GetTextLineHeight();
                    text_render_commands.back().Text += '\n';
                }
                current_draw_position.x = entire_text_bounding_box.GetBL().x;

                std::string chapter_text = "CHAPTER " + std::to_string(verse.Id.ChapterNumber);

                ImVec2 text_size = ImGui::CalcTextSize(chapter_text.c_str());

                TextRenderCommand chapter_title_render_command =
                {
                    .Text = chapter_text + '\n',
                    //.Text = chapter_text,
                    .Color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    .DrawPosition = current_draw_position,
                };
                chapter_title_render_command.TextSize = ImGui::CalcTextSize(chapter_title_render_command.Text.c_str());
                chapter_title_render_command.TextBoundingBox = ImRect(
                    chapter_title_render_command.DrawPosition,
                    chapter_title_render_command.DrawPosition + text_size);
                text_render_commands.push_back(chapter_title_render_command);

                //current_draw_position.x += text_size.x;
                current_draw_position.y += ImGui::GetTextLineHeight();

                previous_chapter_number = verse.Id.ChapterNumber;
                first_chapter = false;
                first_verse = true;
            }

            // PRINT THE VERSE NUMBER.
            {
                std::string verse_number = std::to_string(verse.Id.VerseNumber) + ' ';
                if (first_verse)
                {
                    first_verse = false;
                }
                else
                {
                    verse_number = ' ' + verse_number;
                }

                ImVec2 text_size = ImGui::CalcTextSize(verse_number.c_str());
                ImVec2 text_end_position = current_draw_position + text_size;
                bool text_fits_on_current_line = (text_end_position.x <= entire_text_bounding_box_max_coordinates.x);
                if (!text_fits_on_current_line)
                {
                    current_draw_position.x = entire_text_bounding_box.GetBL().x;
                    current_draw_position.y += ImGui::GetTextLineHeight();

                    //verse_number = '\n' + verse_number;
                    text_render_commands.back().Text += '\n';

                    // Remove any extra spaces from the beginning of the line.
                    verse_number = std::to_string(verse.Id.VerseNumber) + ' ';
                }

                TextRenderCommand verse_number_render_command =
                {
                    .Text = verse_number,
                    .Color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    .DrawPosition = current_draw_position,
                };
                verse_number_render_command.TextSize = ImGui::CalcTextSize(verse_number_render_command.Text.c_str());
                verse_number_render_command.TextBoundingBox = ImRect(
                    verse_number_render_command.DrawPosition,
                    verse_number_render_command.DrawPosition + text_size);
                text_render_commands.push_back(verse_number_render_command);

                current_draw_position.x += text_size.x;
            }

            const std::vector<BIBLE_DATA::Token>* tokens = verse.GetTokens();
            for (const BIBLE_DATA::Token& token : *tokens)
            {
                std::string token_text = token.Text;
                ImVec2 text_size = ImGui::CalcTextSize(token_text.c_str());
                ImVec2 text_end_position = current_draw_position + text_size;
                bool text_fits_on_current_line = 
                    (text_end_position.x <= entire_text_bounding_box_max_coordinates.x) ||
                    // Spaces don't need to start a new line.
                    (BIBLE_DATA::TokenType::SPACE == token.Type);
                if (!text_fits_on_current_line)
                {
                    current_draw_position.x = entire_text_bounding_box.GetBL().x;
                    current_draw_position.y += ImGui::GetTextLineHeight();

                    //token_text = '\n' + token_text;
                    text_render_commands.back().Text += '\n';
                }

                TextRenderCommand token_render_command =
                {
                    .Text = token_text,
                    .Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                    .DrawPosition = current_draw_position,
                };
                token_render_command.TextSize = ImGui::CalcTextSize(token_render_command.Text.c_str());
                token_render_command.TextBoundingBox = ImRect(
                    token_render_command.DrawPosition,
                    token_render_command.DrawPosition + text_size);
                text_render_commands.push_back(token_render_command);

                current_draw_position.x += text_size.x;
            }
        }
        std::string final_rendered_verse_text;
        for (const TextRenderCommand& text_render_command : text_render_commands)
        {
            final_rendered_verse_text += text_render_command.Text;
        }

        // ALLOCATE AN ID FOR REFERENCING THE TEXT.
        ImGuiID text_id = window->GetID("###VerseText");

        // CHANGE THE CURSOR TO INDICATE SELECTABLE TEXT IF THE ITEM IS HOVERABLE.
        ImGuiContext& gui_context = *GImGui;
        bool hovered = ImGui::ItemHoverable(entire_text_bounding_box, text_id);
        if (hovered)
        {
            gui_context.MouseCursor = ImGuiMouseCursor_TextInput;
        }

        // GET THE INPUT TEXT STATE FOR THIS VERSE TEXT BOX.
        ImGuiInputTextState* input_text_state = nullptr;
        bool current_verse_text_being_tracked_for_input = (gui_context.InputTextState.ID == text_id);
        if (current_verse_text_being_tracked_for_input)
        {
            input_text_state = &gui_context.InputTextState;
        }

        // ALLOW THIS TEXT BOX TO RECEIVE FOCUS.
        bool focus_requested = ImGui::FocusableItemRegister(window, text_id);

        // CHECK IF THE USER CLICKED INTO THIS TEXT AREA.
        ImGuiIO& user_input = gui_context.IO;
        constexpr int LEFT_MOUSE_BUTTON = 0;
        bool user_clicked_on_text_area = hovered && user_input.MouseClicked[LEFT_MOUSE_BUTTON];

        // CHECK THE STATE OF SCROLLING IN THE WINDOW.
        bool no_current_active_id = (gui_context.ActiveId == 0);
        ImGuiID vertical_scrollbar_id = ImGui::GetScrollbarID(window, ImGuiAxis_Y);
        bool vertical_scrollbar_active_previously = (gui_context.ActiveIdPreviousFrame == vertical_scrollbar_id);
        bool user_scroll_finish = no_current_active_id && vertical_scrollbar_active_previously;
        bool user_scroll_active = (gui_context.ActiveId == vertical_scrollbar_id);

        // INITIALIZE INFORMATION FOR TRACKING INPUT ON THE TEXT.
        bool clear_active_id = false;
        bool should_activate = (focus_requested || user_clicked_on_text_area || user_scroll_finish);
        bool different_item_previously_active = (gui_context.ActiveId != text_id);
        bool newly_initializing_input_text = (should_activate || user_scroll_active) && different_item_previously_active;
        if (newly_initializing_input_text)
        {
            // INITIALIZE THE INPUT TEXT STATE.
            input_text_state = &gui_context.InputTextState;
            // +1 is needed to be able to select last character.
            int entire_text_length = static_cast<int>(final_rendered_verse_text.length() + 1);

            // TextW is used for copying, so it needs to be populated.
            constexpr int ALLOW_AT_LEAST_1_CHARACTER_FOR_EMPTY_STRING = 1;
            input_text_state->TextW.resize(entire_text_length + ALLOW_AT_LEAST_1_CHARACTER_FOR_EMPTY_STRING);
            constexpr char* USE_IMPLICIT_NULL_TERMINATOR = nullptr;
            input_text_state->CurLenW = ImTextStrFromUtf8(
                input_text_state->TextW.Data, 
                entire_text_length, 
                final_rendered_verse_text.c_str(), 
                USE_IMPLICIT_NULL_TERMINATOR);

            input_text_state->ID = text_id;
            input_text_state->ScrollX = 0.0f;

            constexpr bool IS_MULTI_LINE = false;
            stb_textedit_initialize_state(&input_text_state->Stb, IS_MULTI_LINE);

            // ACTIVATE THE WINDOW
            ImGui::SetActiveID(text_id, window);
            ImGui::SetFocusID(text_id, window);
            ImGui::FocusWindow(window);
        }
        else if (user_input.MouseClicked[LEFT_MOUSE_BUTTON])
        {
            // RELEASE FOCUS WHEN CLICKED OUTSIDE THE WINDOW.
            clear_active_id = true;
        }

        // PROCESS MOUSE INPUT IF THE TEXT BOX IS ACTIVE.
        bool current_text_box_active = (gui_context.ActiveId == text_id);
        if (current_text_box_active)
        {
            // CALCULATE THE MOUSE POSITION WITHIN THE WINDOW.
            float mouse_x = (user_input.MousePos.x - entire_text_bounding_box.Min.x - gui_context.Style.FramePadding.x + input_text_state->ScrollX);
            float mouse_y = (user_input.MousePos.y - window->DC.CursorPos.y - gui_context.Style.FramePadding.y);

            // DETERMINE THE KIND OF MOUSE INTERACTION THAT'S OCCURRED.
            bool user_double_clicked_on_text_area = hovered && user_input.MouseDoubleClicked[LEFT_MOUSE_BUTTON];
            bool user_clicked_when_not_already_selecting_all = user_input.MouseClicked[LEFT_MOUSE_BUTTON] && !input_text_state->SelectedAllMouseLock;
            bool user_dragged_over_text = (
                user_input.MouseDown[LEFT_MOUSE_BUTTON] && 
                !input_text_state->SelectedAllMouseLock && 
                (user_input.MouseDelta.x != 0.0f || user_input.MouseDelta.y != 0.0f));
            if (user_double_clicked_on_text_area)
            {
                // SELECT ALL TEXT.
                input_text_state->SelectAll();
                input_text_state->SelectedAllMouseLock = true;
            }
            else if (user_clicked_when_not_already_selecting_all)
            {
                if (hovered)
                {
                    stb_textedit_click(input_text_state, &input_text_state->Stb, mouse_x, mouse_y);
                    input_text_state->CursorAnimReset();
                }
            }
            else if (user_dragged_over_text)
            {
                stb_textedit_drag(input_text_state, &input_text_state->Stb, mouse_x, mouse_y);
                input_text_state->CursorAnimReset();
                input_text_state->CursorFollow = true;
            }

            // RELEASE LOCKING OF SELECT ALL IF THE USER RELEASED THE MOUSE AFTERWARDS.
            bool select_all_mouse_button_released = input_text_state->SelectedAllMouseLock && !user_input.MouseDown[LEFT_MOUSE_BUTTON];
            if (select_all_mouse_button_released)
            {
                input_text_state->SelectedAllMouseLock = false;
            }
        }

        // COMPUTE WHAT MAY NEED TO BE RENDERED.
        bool render_cursor = current_text_box_active || (input_text_state && user_scroll_active);
        bool render_selection = render_cursor && input_text_state && input_text_state->HasSelection();

        // HANDLE KEYBOARD INPUT.
        bool text_box_will_remain_active_and_did_not_just_activate = current_text_box_active && !gui_context.ActiveIdIsJustActivated && !clear_active_id;
        if (text_box_will_remain_active_and_did_not_just_activate)
        {
            // DETERMINE IF SPECIAL KEYS WERE PRESSED.
            bool is_shortcut_key = user_input.KeyCtrl;
            bool is_copy = is_shortcut_key && ImGui::IsKeyPressedMap(ImGuiKey_C);
            bool is_select_all = is_shortcut_key && ImGui::IsKeyPressedMap(ImGuiKey_A);

            // PROCESS DIFFERENT KEY PRESSES.
            if (ImGui::IsKeyPressedMap(ImGuiKey_LeftArrow))
            {
                input_text_state->OnKeyPressed(STB_TEXTEDIT_K_LEFT);
            }
            else if (ImGui::IsKeyPressedMap(ImGuiKey_RightArrow))
            {
                input_text_state->OnKeyPressed(STB_TEXTEDIT_K_RIGHT);
            }
            else if (ImGui::IsKeyPressedMap(ImGuiKey_UpArrow))
            {
                input_text_state->OnKeyPressed(STB_TEXTEDIT_K_UP);
            }
            else if (ImGui::IsKeyPressedMap(ImGuiKey_DownArrow))
            {
                input_text_state->OnKeyPressed(STB_TEXTEDIT_K_DOWN);
            }
            else if (is_select_all)
            {
                input_text_state->SelectAll();
                input_text_state->CursorFollow = true;
            }
            else if (is_copy)
            {
                // MAKE SURE A CLIPBOARD IS SUPPORT.
                if (user_input.SetClipboardTextFn)
                {
                    // COMPUTE THE RANGE OF SELECTED TEXT.
                    int selected_text_begin = 0;
                    int selected_text_end = input_text_state->CurLenW;
                    if (input_text_state->HasSelection())
                    {
                        selected_text_begin = ImMin(input_text_state->Stb.select_start, input_text_state->Stb.select_end);
                        selected_text_end = ImMax(input_text_state->Stb.select_start, input_text_state->Stb.select_end);
                    }
                    
                    const ImWchar* text_beginning = input_text_state->TextW.Data + selected_text_begin;
                    const ImWchar* text_ending = input_text_state->TextW.Data + selected_text_end;
                    int clipboard_data_length = ImTextCountUtf8BytesFromStr(text_beginning, text_ending);
                    constexpr int ROOM_FOR_NULL_TERMINATOR = 1;
                    clipboard_data_length += ROOM_FOR_NULL_TERMINATOR;

                    // COPY DATA TO THE CLIPBOARD.
                    char* clipboard_data = (char*)IM_ALLOC(clipboard_data_length * sizeof(char));
                    ImTextStrToUtf8(clipboard_data, clipboard_data_length, text_beginning, text_ending);
                    ImGui::SetClipboardText(clipboard_data);
                    ImGui::MemFree(clipboard_data);
                }
            }

            // ALLOW UPDATED INPUT TO AFFECT RENDERING OF A SELECTION.
            render_selection |= input_text_state->HasSelection() && render_cursor;
        }

        // RELEASE THE ACTIVE ID ON THIS TEXT BOX IF APPROPRIATE.
        bool current_text_box_active_but_should_be_released = clear_active_id && current_text_box_active;
        if (current_text_box_active_but_should_be_released)
        {
            ImGui::ClearActiveID();
        }

        // DETERMINE IF SPECIAL TEXT INPUT RENDERING SHOULD BE APPLIED.
        bool render_text_input_features = render_cursor || render_selection;
        if (render_text_input_features)
        {
            // COMPUTE THE CLIPPING RECTANGLE.
            const ImVec4 clip_rectangle(
                entire_text_bounding_box.Min.x,
                entire_text_bounding_box.Min.y,
                entire_text_bounding_box.Min.x + window_content_region_size.x,
                entire_text_bounding_box.Min.y + window_content_region_size.y);

            // INTIALIZE VARIABLES RELATED TO DRAWING THE SELECTION AND CURSOR.
            ImVec2 draw_position = window->DC.CursorPos;
            const ImVec2 draw_scroll = ImVec2(input_text_state->ScrollX, 0.0f);
            ImVec2 cursor_offset;

            // RENDER THE SELECTION IF APPLICABLE.
            if (render_selection)
            {
                // COMPUTE THE OFFSET FOR THE SELECTION.
                ImVec2 select_start_offset;
                const ImWchar* text_begin = input_text_state->TextW.Data;

                constexpr int LINE_NUMBER_NOT_FOUND = -1;
                int searches_remaining = 0;

                const ImWchar* character_straddling_cursor = nullptr;
                int line_number_straddling_cursor = -1000;
                if (render_cursor)
                {
                    character_straddling_cursor = text_begin + input_text_state->Stb.cursor;
                    line_number_straddling_cursor = LINE_NUMBER_NOT_FOUND;
                    ++searches_remaining;
                }

                const ImWchar* character_straddling_selection_start = nullptr;
                int line_number_straddling_selection_start = -1000;
                if (render_selection)
                {
                    character_straddling_selection_start = text_begin + ImMin(input_text_state->Stb.select_start, input_text_state->Stb.select_end);
                    line_number_straddling_selection_start = LINE_NUMBER_NOT_FOUND;
                    ++searches_remaining;
                }

                ++searches_remaining;
                int line_count = 0;
                constexpr char NULL_TERMINATOR = 0;
                for (const ImWchar* character = text_begin; *character != NULL_TERMINATOR; ++character)
                {
                    // COUNT ANY NEWLINES.
                    bool is_newline = *character == '\n';
                    if (is_newline)
                    {
                        ++line_count;

                        // UPDATE THE LINE STRADDLING THE CURSOR.
                        bool line_number_straddling_cursor_not_found = line_number_straddling_cursor == LINE_NUMBER_NOT_FOUND && character >= character_straddling_cursor;
                        if (line_number_straddling_cursor_not_found)
                        { 
                            line_number_straddling_cursor = line_count;

                            // STOP IF WE SHOULDN'T SEARCH ANYMORE.
                            if (--searches_remaining <= 0)
                            {
                                break;
                            }
                        }

                        // UPDATE THE LINE STRADDLING THE SELECTION.
                        bool line_number_straddling_selection_not_found = line_number_straddling_selection_start == LINE_NUMBER_NOT_FOUND && character >= character_straddling_selection_start;
                        if (line_number_straddling_selection_not_found)
                        { 
                            line_number_straddling_selection_start = line_count; 

                            // STOP IF WE SHOULDN'T SEARCH ANYMORE.
                            if (--searches_remaining <= 0)
                            {
                                break;
                            }
                        }
                    }
                }
                // MAKE SURE SOME LINE NUMBERS WERE FOUND.
                line_count++;
                if (line_number_straddling_cursor == LINE_NUMBER_NOT_FOUND)
                {
                    line_number_straddling_cursor = line_count;
                }
                if (line_number_straddling_selection_start == LINE_NUMBER_NOT_FOUND)
                {
                    line_number_straddling_selection_start = line_count;
                }

                // CALCULATE OFFSETS TO THE CURSOR AND SELECTION.
                cursor_offset.x = InputTextCalcTextSizeW(
                    ImStrbolW(character_straddling_cursor, text_begin), 
                    character_straddling_cursor).x;
                cursor_offset.y = line_number_straddling_cursor * gui_context.FontSize;
                if (line_number_straddling_selection_start >= 0)
                {
                    select_start_offset.x = InputTextCalcTextSizeW(
                        ImStrbolW(character_straddling_selection_start, text_begin), 
                        character_straddling_selection_start).x;
                    select_start_offset.y = line_number_straddling_selection_start * gui_context.FontSize;
                }

                // HANDLE SCROLLING FOR THE CURSOR.
                bool following_and_rendering_cursor = render_cursor && input_text_state->CursorFollow;
                if (following_and_rendering_cursor)
                {
                    // RESET PARTS OF THE INPUT TEXT STATE..
                    input_text_state->ScrollX = 0.0f;
                    input_text_state->CursorFollow = false;

                    // CALCULATE THE SIZE OF THE CURRENT TEXT ITEM.
                    // This is needed for proper, stable scrolling.
                    ImVec2 text_item_size = ImGui::CalcItemSize(
                        ImVec2(),
                        ImGui::CalcItemWidth(),
                        (ImGui::GetTextLineHeight() * 8.0f) + gui_context.Style.FramePadding.y * 2.0f);

                    // UPDATE VERTICAL SCROLLING.
                    float scroll_y = window->Scroll.y;
                    bool cursor_before_window_scroll_position = cursor_offset.y - gui_context.FontSize < scroll_y;
                    bool cursor_at_or_after_window_scroll_position = cursor_offset.y - text_item_size.y >= scroll_y;
                    if (cursor_before_window_scroll_position)
                    {
                        // KEEP THE CURSOR IN VIEW OF THE WINDOW'S SCROLLING.
                        scroll_y = ImMax(0.0f, cursor_offset.y - gui_context.FontSize);
                    }
                    else if (cursor_at_or_after_window_scroll_position)
                    {
                        scroll_y = cursor_offset.y - text_item_size.y;
                    }
                    // UPDATE THE DRAW POSITION FOR THE CURSOR.
                    draw_position.y += (window->Scroll.y - scroll_y);

                    // UPDATE THE WINDOW SCROLL AMOUNT.
                    window->Scroll.y = scroll_y;
                }

                // RENDER SHADED RECTANGLES BEHIND THE SELECTED TEXT.
                const ImWchar* text_selected_begin = text_begin + ImMin(input_text_state->Stb.select_start, input_text_state->Stb.select_end);
                const ImWchar* text_selected_end = text_begin + ImMax(input_text_state->Stb.select_start, input_text_state->Stb.select_end);
                ImU32 selected_text_background_color = ImGui::GetColorU32(ImGuiCol_TextSelectedBg);
                float background_top_offset = 0.0f;
                float background_bottom_offset = 0.0f;
                ImVec2 rectangle_position = draw_position + select_start_offset - draw_scroll;
                for (const ImWchar* character = text_selected_begin; character < text_selected_end;)
                {
                    // CHECK IF THERE'S MORE ROOM BEFORE THE TOP OF THE CLIP RECTANGLE.
                    bool room_before_top_of_clip_rectangle = rectangle_position.y < clip_rectangle.y;
                    if (room_before_top_of_clip_rectangle)
                    {
                        // MOVE TO THE END OF THE TEXT OR THE NEXT NEWLINE.
                        while (character < text_selected_end)
                        {
                            // BREAK OUT OF THE LOOP IF WE SHOULD MOVE TO A NEW LINE.
                            if (*character++ == '\n')
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        // RENDER A SHADED RECTANGLE FOR THE BACKGROUND.
                        ImVec2* no_offset_output_needed = nullptr;
                        constexpr bool STOP_ON_NEW_LINES = true;
                        ImVec2 rectangle_size = InputTextCalcTextSizeW(character, text_selected_end, &character, no_offset_output_needed, STOP_ON_NEW_LINES);
                        // MAKE SURE SELECTED EMPTY LINES CAN BE SEEN.
                        bool line_is_empty = rectangle_size.x <= 0.0f;
                        if (line_is_empty)
                        {
                            // Use half the width of a space for empty lines.
                            float space_character_advance = gui_context.Font->GetCharAdvance((ImWchar)' ');
                            rectangle_size.x = space_character_advance * 0.5f;
                        }

                        ImRect background_rectangle(
                            rectangle_position + ImVec2(0.0f, background_top_offset - gui_context.FontSize), 
                            rectangle_position + ImVec2(rectangle_size.x, background_bottom_offset));
                        window->DrawList->AddRectFilled(background_rectangle.Min, background_rectangle.Max, selected_text_background_color);
                    }

                    // MOVE THE RECTANGLE TO THE NEXT POSITION.
                    rectangle_position.x = draw_position.x - draw_scroll.x;
                    rectangle_position.y += gui_context.FontSize;
                }
            }

            // DRAW A BLINKING CURSOR IF APPLICABLE.
            if (render_cursor)
            {
                // UPDATE THE CURSOR BLINKING.
                input_text_state->CursorAnim += user_input.DeltaTime;

                // DETERMINE IF THE CURSOR SHOULD BE VISIBLE.
                // The numbers used here reflect what the actual ImGui code does.
                bool cursor_is_visible = (input_text_state->CursorAnim <= 0.0f) || ImFmod(input_text_state->CursorAnim, 1.20f) <= 0.80f;
                if (cursor_is_visible)
                {
                    // DRAW THE CURSOR.
                    ImVec2 cursor_screen_position = draw_position + cursor_offset - draw_scroll;
                    ImRect cursor_screen_rectangle(
                        cursor_screen_position.x,
                        /// @todo   Why does the actual ImGui code use this formula?
                        cursor_screen_position.y - gui_context.FontSize + 0.5f, cursor_screen_position.x + 1.0f, cursor_screen_position.y - 1.5f);
                    window->DrawList->AddLine(cursor_screen_rectangle.Min, cursor_screen_rectangle.GetBL(), ImGui::GetColorU32(ImGuiCol_Text));
                }
            }
        }

        // RENDER ALL OF THE TEXT.
        for (const TextRenderCommand& text_render_command : text_render_commands)
        {
            // CALCULATE THE BOUNDING BOX FOR THE TEXT.
            /// @todo Adding items here messed a bit with scrolling.
            // ItemSize adds scrolling
#if 1
            ImGui::ItemSize(text_render_command.TextSize);
#endif

            std::string lowercase_word = text_render_command.Text;
            std::transform(
                lowercase_word.begin(),
                lowercase_word.end(),
                lowercase_word.begin(),
                [](const char character) { return static_cast<char>(std::tolower(character)); });

            // GET THE COLOR.
            ImVec4 color = text_render_command.Color;
            auto current_word_color = colors_by_word.find(lowercase_word);
            bool current_word_color_exists = (colors_by_word.cend() != current_word_color);
            if (current_word_color_exists)
            {
                color = current_word_color->second;
            }

            // SET THE TEXT COLOR.
            // A temporarily different (usually) highlight color is used when hovering over.
#if 0
            bool mouse_over_text_bounding_box = ImGui::IsItemHovered();
#else
            bool mouse_over_text_bounding_box = text_render_command.TextBoundingBox.Contains(gui_context.IO.MousePos);
#endif
            bool is_currently_highlighted_word = (lowercase_word == CurrentlyHighlightedWord) || mouse_over_text_bounding_box;
            if (is_currently_highlighted_word)
            {
                /// @todo   Something other than yellow for highlights?
                color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

                CurrentlyHighlightedWord = lowercase_word;
            }
                   
            ImU32 packed_color = ImGui::ColorConvertFloat4ToU32(color);

            // RENDER THE TEXT.
            constexpr char* USE_IMPLICIT_NULL_TERMINATOR = nullptr;
            window->DrawList->AddText(
                gui_context.Font,
                gui_context.FontSize,
                text_render_command.DrawPosition,
                packed_color,
                text_render_command.Text.c_str(),
                USE_IMPLICIT_NULL_TERMINATOR);
        }
    }

    /// Updates the lookup of colors for the current set of verses.
    /// @param[in]  colors_by_word - The color lookup to update.
    void BibleVersesWindow::UpdateColorLookup(std::map<std::string, ImVec4>& colors_by_word)
    {
        // KJV occurrence counts are in comments beside some words.
        static const std::unordered_set<std::string> LOWERCASE_STOP_WORDS =
        {
            "a", // 8177
            "about",
            "again",
            "against",
            "also",
            "an",
            "any",
            "and", // 51696
            "are",
            "art",
            "as",
            "at",
            "base",
            "be", // 7013
            "because",
            "become",
            "been",
            "before",
            "being",
            "better",
            "bring",
            "brings",
            "bringeth",
            "by",
            "but",
            "came",
            "come",
            "did",
            "didst",
            "do",
            "does",
            "dost",
            "doth",
            "down",
            "even",
            "ever",
            "express",
            "first",
            "fold",
            "found",
            "for", // 8971
            "forth",
            "from",
            "had",
            "has",
            "hast",
            "hath",
            "have",
            "having",
            "he", // 10420
            "him", // 6659
            "himself",
            "his", // 8473
            "i", // 8854
            "in", // 12667
            "indeed",
            "into",
            "is", // 6989
            "it", // 6129
            "its",
            "laid",
            "let",
            "like",
            "made",
            "make",
            "makes",
            "maketh",
            "making",
            "many",
            "may",
            "me",
            "more",
            "much",
            "my",
            "no",
            "not", // 6596
            "now",
            "o",
            "of", // 34617
            "on",
            "only",
            "or",
            "our",
            "out",
            "put",
            "said",
            "saith",
            "same",
            "sat",
            "say",
            "saying",
            "says",
            "seat",
            "send",
            "sending",
            "sent",
            "shalt",
            "sit",
            "so",
            "t",
            "take",
            "than",
            "that", // 12912
            "the", // 63924
            "thee",
            "their",
            "theirs",
            "them", // 6430
            "therefore",
            "these",
            "they", // 7376
            "thine",
            "things",
            "this",
            "those",
            "thou",
            "through",
            "thy",
            "till",
            "to", // 13562
            "together",
            "took",
            "under",
            "until",
            "unto",
            "up",
            "us",
            "very",
            "was",
            "when",
            "which", // 4413
            "who",
            "whom",
            "whose",
            "with", // 6012
            "you",
            "your"
        };

        // UPDATE COLORS FOR ANY UNCOLORED WORDS.
        /// @todo   Better centralize this code!
        std::random_device random_number_generator;
        ImVec4 color = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f };
        for (const auto& translation_name_and_verses : VersesByTranslationName)
        {
            /// @todo   Check user settings for what's displayed?

            for (const auto& verse : translation_name_and_verses.second)
            {
                const std::vector<BIBLE_DATA::Token>* verse_tokens = verse.GetTokens();
                for (const auto& token : *verse_tokens)
                {
                    // SKIP NON-WORD TOKENS.
                    bool is_word = (BIBLE_DATA::TokenType::WORD == token.Type);
                    if (!is_word)
                    {
                        continue;
                    }

                    std::string lowercase_word = token.Text;
                    std::transform(
                        lowercase_word.begin(),
                        lowercase_word.end(),
                        lowercase_word.begin(),
                        [](const char character) { return static_cast<char>(std::tolower(character)); });

                    // SKIP STORING COLORS FOR OVERLY COMMON WORDS.
#if __EMSCRIPTEN__
                    unsigned int current_word_count_in_stop_words = LOWERCASE_STOP_WORDS.count(lowercase_word);
                    bool is_stop_word = (current_word_count_in_stop_words > 0);
#else
                    bool is_stop_word = LOWERCASE_STOP_WORDS.contains(lowercase_word);
#endif
                    if (is_stop_word)
                    {
                        continue;
                    }

                    // UPDATE THE CURRENT WORDS COLOR.
                    /// @todo   How to preserve colors while not re-using colors?
                    /// @todo   Upper/lowercase?
                    auto current_word_color = colors_by_word.find(lowercase_word);
                    bool current_word_color_exists = (colors_by_word.cend() != current_word_color);
                    if (!current_word_color_exists)
                    {
                        // ASSIGN SOME RANDOM COLORS.
                        color.x = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                        color.y = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                        color.z = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());

                        colors_by_word[lowercase_word] = color;
                    }
                }
            }
        }
    }

    /// Computes statistics about the words currently displayed in the window.
    void BibleVersesWindow::ComputeWordStatistics()
    {
        WordsByDecreasingCount.clear();

        std::map<std::string, unsigned int> occurrence_counts_by_word;

        for (const auto& translation_name_and_verses : VersesByTranslationName)
        {
            /// @todo   Check user settings for what's displayed?
            for (const BIBLE_DATA::BibleVerse& verse : translation_name_and_verses.second)
            {
                const std::vector<BIBLE_DATA::Token>* verse_tokens = verse.GetTokens();
                for (const auto& token : *verse_tokens)
                {
                    // SKIP NON-WORD TOKENS.
                    bool is_word = (BIBLE_DATA::TokenType::WORD == token.Type);
                    if (!is_word)
                    {
                        continue;
                    }

                    std::string lowercase_word = token.Text;
                    std::transform(
                        lowercase_word.begin(),
                        lowercase_word.end(),
                        lowercase_word.begin(),
                        [](const char character) { return static_cast<char>(std::tolower(character)); });

                    // COUNT THE WORD.
                    ++occurrence_counts_by_word[lowercase_word];
                }
            }
        }

        for (const auto& word_and_count : occurrence_counts_by_word)
        {
            WordsByDecreasingCount.emplace_back(
                std::make_pair(word_and_count.first, word_and_count.second)
            );
        }
        std::sort(
            WordsByDecreasingCount.begin(),
            WordsByDecreasingCount.end(),
            [](const WordAndCount& left, const WordAndCount& right) -> bool
            {
                return left.second > right.second;
            });
    }
}
