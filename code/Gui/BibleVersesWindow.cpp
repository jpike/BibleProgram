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
    void BibleVersesWindow::UpdateAndRender()
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        // RENDER EACH VERSE IN THE WINDOW.
        std::string window_title = (
            BIBLE_DATA::BibleBook::FullName(StartingVerseId.Book) + " " +
            std::to_string(StartingVerseId.ChapterNumber) + ":" +
            std::to_string(StartingVerseId.VerseNumber));
        bool multiple_verses = Verses.size() > 1;
        if (multiple_verses)
        {
            window_title += " - " +
                BIBLE_DATA::BibleBook::FullName(EndingVerseId.Book) + " " +
                std::to_string(EndingVerseId.ChapterNumber) + ":" +
                std::to_string(EndingVerseId.VerseNumber);
        }
        std::string window_title_and_id = window_title + "###Verses";

        ImVec2 min_window_size_in_pixels = { 400.0f, 400.0f };
        ImGui::SetNextWindowSize(min_window_size_in_pixels, ImGuiCond_FirstUseEver);

        if (ImGui::Begin(window_title_and_id.c_str(), &Open, ImGuiWindowFlags_MenuBar))
        {
            // RENDER A MENU BAR.
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::Button("Colorize"))
                {
                    UpdateColorLookup();
                }

                ImGui::EndMenuBar();
            }

            UpdateAndRenderVerseContent(Verses);
        }
        ImGui::End();

#if 0
        static std::vector<std::pair<unsigned int, std::string>> counts_with_words;

        /// @todo   No scrollbar is due to child frame?
        if (ImGui::Begin(window_title_and_id.c_str(), &Open, ImGuiWindowFlags_MenuBar /*| ImGuiWindowFlags_NoScrollbar*/))
        {
            // RENDER A MENU BAR.
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::Button("Statistics"))
                {
                    ComputeWordStatistics();
                    counts_with_words.clear();
                    for (const auto& word_with_count : OccurrenceCountsByWord)
                    {
                        counts_with_words.emplace_back(word_with_count.second, word_with_count.first);
                    }
                    std::sort(counts_with_words.begin(), counts_with_words.end());
                    std::reverse(counts_with_words.begin(), counts_with_words.end());
                }

                if (ImGui::Button("Colorize"))
                {
                    UpdateColorLookup();
                }

                ImGui::EndMenuBar();
            }

            // RENDER THE VERSES.
            //ImGui::BeginGroup();

            std::string entire_verse_text;
            for (const auto& verse : Verses)
            {
                auto tokens = verse.GetTokens();
                for (const auto& token : *tokens)
                {
                    entire_verse_text += token.Text;
                    if (token.Type != BIBLE_DATA::TokenType::SPACE)
                    {
                        entire_verse_text += " ";
                    }
                }
            }

            BIBLE_DATA::BibleBookId previous_book = BIBLE_DATA::BibleBookId::INVALID;
            unsigned int previous_chapter_number = 0;
            for (const auto& verse : Verses)
            {
                // PRINT SOME TEXT TO INDICATE IF A NEW BOOK IS BEING STARTED.
                bool new_book_being_started = (previous_book != verse.Id.Book);
                if (new_book_being_started)
                {
                    std::string book_name = BIBLE_DATA::BibleBook::FullName(verse.Id.Book);
                    std::transform(
                        book_name.begin(), 
                        book_name.end(), 
                        book_name.begin(), 
                        [](const char character) { return static_cast<char>(std::toupper(character)); });
                    ImGui::TextDisabled(book_name.c_str());

                    previous_book = verse.Id.Book;
                }
                // PRINT SOME TEXT TO INDICATE IF A NEW CHAPTER IS BEING STARTED.
                bool new_chapter_being_started = (previous_chapter_number != verse.Id.ChapterNumber);
                if (new_chapter_being_started)
                {
                    std::string chapter_text = "CHAPTER " + std::to_string(verse.Id.ChapterNumber);
                    ImGui::TextDisabled(chapter_text.c_str());

                    previous_chapter_number = verse.Id.ChapterNumber;
                }

                bool header_info_printed = (new_book_being_started || new_chapter_being_started);
                if (header_info_printed)
                {
                    ImGui::NewLine();
                }

                Render(verse);
            }

            bool any_verse_hovered = ImGui::IsAnyItemHovered();
            if (!any_verse_hovered)
            {
                CurrentlyHighlightedWord.clear();
            }
        }
        ImGui::End();

        /// @todo   Make this a separate window!
        if (ImGui::Begin("Word Counts"))
        {
            for (const auto& count_with_word : counts_with_words)
            {
                std::string word_statistics = count_with_word.second + ": " + std::to_string(count_with_word.first);
                ImGui::Text(word_statistics.c_str());
            }
        }
        ImGui::End();
#endif
    }

    struct TextRenderCommand
    {
        std::string Text = "";
        ImVec4 Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec2 DrawPosition = ImVec2(0.0f, 0.0f);
        //bool EndsWithNewLine = false;
    };

    /// Updates and renders the specified verse content in the window.
    /// Some of the implementation here is based on ImGui's multi-line text input
    /// but modified/simplified for our purposes here.
    /// @param[in]  verses - The verse content to render.
    void BibleVersesWindow::UpdateAndRenderVerseContent(const std::vector<BIBLE_DATA::BibleVerse>& verses)
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
        for (const BIBLE_DATA::BibleVerse& verse : verses)
        {
            // PRINT SOME TEXT TO INDICATE IF A NEW BOOK IS BEING STARTED.
            bool new_book_being_started = (previous_book != verse.Id.Book);
            if (new_book_being_started)
            {
                std::string book_name = BIBLE_DATA::BibleBook::FullName(verse.Id.Book);
                std::transform(
                    book_name.begin(),
                    book_name.end(),
                    book_name.begin(),
                    [](const char character) { return static_cast<char>(std::toupper(character)); });

                ImVec2 text_size = ImGui::CalcTextSize(book_name.c_str());
                ImVec2 text_end_position = current_draw_position + text_size;
                bool text_fits_on_current_line = (text_end_position.x <= entire_text_bounding_box_max_coordinates.x);
                if (!text_fits_on_current_line)
                {
                    current_draw_position.x = entire_text_bounding_box.GetBL().x;
                    current_draw_position.y += ImGui::GetTextLineHeight();

                    //book_name = '\n' + book_name;
                    text_render_commands.back().Text += '\n';
                }

                TextRenderCommand book_title_render_command =
                {
                    .Text = book_name + '\n',
                    .Color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    .DrawPosition = current_draw_position,
                };
                text_render_commands.push_back(book_title_render_command);

                //current_draw_position.x += text_size.x;
                current_draw_position.y += ImGui::GetTextLineHeight();

                previous_book = verse.Id.Book;
            }
            // PRINT SOME TEXT TO INDICATE IF A NEW CHAPTER IS BEING STARTED.
            bool new_chapter_being_started = (previous_chapter_number != verse.Id.ChapterNumber);
            if (new_chapter_being_started)
            {
                std::string chapter_text = "CHAPTER " + std::to_string(verse.Id.ChapterNumber);

                ImVec2 text_size = ImGui::CalcTextSize(chapter_text.c_str());
                ImVec2 text_end_position = current_draw_position + text_size;
                bool text_fits_on_current_line = (text_end_position.x <= entire_text_bounding_box_max_coordinates.x);
                if (!text_fits_on_current_line)
                {
                    current_draw_position.x = entire_text_bounding_box.GetBL().x;
                    current_draw_position.y += ImGui::GetTextLineHeight();

                    //chapter_text = '\n' + chapter_text;
                    text_render_commands.back().Text += '\n';
                }

                TextRenderCommand chapter_title_render_command =
                {
                    .Text = chapter_text + '\n',
                    .Color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    .DrawPosition = current_draw_position,
                };
                text_render_commands.push_back(chapter_title_render_command);

                //current_draw_position.x += text_size.x;
                current_draw_position.y += ImGui::GetTextLineHeight();

                previous_chapter_number = verse.Id.ChapterNumber;
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
                }

                TextRenderCommand verse_number_render_command =
                {
                    .Text = verse_number,
                    .Color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    .DrawPosition = current_draw_position,
                };
                text_render_commands.push_back(verse_number_render_command);

                current_draw_position.x += text_size.x;
            }

            const std::vector<BIBLE_DATA::Token>* tokens = verse.GetTokens();
            for (const BIBLE_DATA::Token& token : *tokens)
            {
                std::string token_text = token.Text;
                ImVec2 text_size = ImGui::CalcTextSize(token_text.c_str());
                ImVec2 text_end_position = current_draw_position + text_size;
                bool text_fits_on_current_line = (text_end_position.x <= entire_text_bounding_box_max_coordinates.x);
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
            int entire_text_length = static_cast<int>(final_rendered_verse_text.length());

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

                    // UPDATE VERTICAL SCROLLING.
                    float scroll_y = window->Scroll.y;
                    bool cursor_before_window_scroll_position = cursor_offset.y - gui_context.FontSize < scroll_y;
                    bool cursor_at_or_after_window_scroll_position = cursor_offset.y - window_content_region_size.y >= scroll_y;
                    if (cursor_before_window_scroll_position)
                    {
                        // KEEP THE CURSOR IN VIEW OF THE WINDOW'S SCROLLING.
                        scroll_y = ImMax(0.0f, cursor_offset.y - gui_context.FontSize);
                    }
                    else if (cursor_at_or_after_window_scroll_position)
                    {
                        scroll_y = cursor_offset.y - window_content_region_size.y;
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
                    // STOP RENDERING IF WE'VE GONE BEYONG THE MAX BOUNDS OF THE CLIP RECTANGLE.
                    bool moved_outside_bounds_of_clip_rectangle = rectangle_position.y > clip_rectangle.w + gui_context.FontSize;
                    if (moved_outside_bounds_of_clip_rectangle)
                    {
                        break;
                    }

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
                        // const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining, ImVec2* out_offset, bool stop_on_new_line)
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
                        background_rectangle.ClipWith(clip_rectangle);
                        if (background_rectangle.Overlaps(clip_rectangle))
                        {
                            window->DrawList->AddRectFilled(background_rectangle.Min, background_rectangle.Max, selected_text_background_color);
                        }
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
                    // DETERMINE IF THE CURSOR IS WITHIN THE CLIP RECTANGLE.
                    ImVec2 cursor_screen_position = draw_position + cursor_offset - draw_scroll;
                    ImRect cursor_screen_rectangle(
                        cursor_screen_position.x,
                        /// @todo   Why does the actual ImGui code use this formula?
                        cursor_screen_position.y - gui_context.FontSize + 0.5f, cursor_screen_position.x + 1.0f, cursor_screen_position.y - 1.5f);
                    if (cursor_screen_rectangle.Overlaps(clip_rectangle))
                    {
                        // DRAW THE CURSOR.
                        window->DrawList->AddLine(cursor_screen_rectangle.Min, cursor_screen_rectangle.GetBL(), ImGui::GetColorU32(ImGuiCol_Text));
                    }
                }
            }
        }

        // RENDER ALL OF THE TEXT.
        for (const TextRenderCommand& text_render_command : text_render_commands)
        {
            // GET THE COLOR.
            ImVec4 color = text_render_command.Color;
            auto current_word_color = ColorsByWord.find(text_render_command.Text);
            bool current_word_color_exists = (ColorsByWord.cend() != current_word_color);
            if (current_word_color_exists)
            {
                color = current_word_color->second;
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

    /// Renders a single Bible verse in the window.
    /// @param[in]  verse - The verse to render.
    void BibleVersesWindow::Render(const BIBLE_DATA::BibleVerse& verse)
    {
        //ImVec4 dark_gray_verse_number_color = { 0.5f, 0.5f, 0.5f, 1.0f };
        std::string verse_number = std::to_string(verse.Id.VerseNumber);
        ImGui::SameLine();
        ImGui::TextDisabled(verse_number.c_str());

        // MAIN LOGIC.
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec4 color;
        color.x = 1.0f;
        color.y = 1.0f;
        color.z = 1.0f;
        color.w = 1.0f;
        const std::vector<BIBLE_DATA::Token>* verse_tokens = verse.GetTokens();
        for (std::size_t token_index = 0; token_index < verse_tokens->size(); ++token_index)
        {
            const auto& token = verse_tokens->at(token_index);
            if (token.Type == BIBLE_DATA::TokenType::SPACE)
            {
                continue;
            }

            // GET THE CURRENT WORD'S COLOR.
            auto current_word_color = ColorsByWord.find(token.Text);
            bool current_word_color_exists = (ColorsByWord.cend() != current_word_color);
            if (current_word_color_exists)
            {
                color = current_word_color->second;
            }
            else
            {
                // White.
                color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            }

            ImGuiContext& gui_context = *GImGui;
            int text_length = ImFormatString(gui_context.TempBuffer, IM_ARRAYSIZE(gui_context.TempBuffer), token.Text.c_str());
            const char* text_begin = gui_context.TempBuffer;
            const char* text_end = text_begin + text_length;

            /// @todo   The code here is very hacky...Need to spend more time refining it.
            ImGui::SameLine();
            ImVec2 text_position(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
            constexpr float WRAP_TO_END_OF_WINDOW = 0.0f;
            float wrap_width = ImGui::CalcWrapWidthForPos(window->DC.CursorPos, WRAP_TO_END_OF_WINDOW);
            ImVec2 text_size = ImGui::CalcTextSize(text_begin, text_end, false, wrap_width);
            ImRect text_bounding_box(text_position, text_position + text_size);

            // Accounting for the spacing between items is very important for proper wrapping.
            float text_width = text_bounding_box.GetWidth() + gui_context.Style.ItemSpacing.x;
            bool current_text_can_fit_on_line = (text_width < wrap_width);
            if (!current_text_can_fit_on_line)
            {
                // START A NEW LINE.
                ImGui::NewLine();

                // RECALCULATE THE BOUNDING BOX FOR THE TEXT FOR THE NEW LINE.
                text_position = ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
                wrap_width = ImGui::CalcWrapWidthForPos(window->DC.CursorPos, WRAP_TO_END_OF_WINDOW);
                text_size = ImGui::CalcTextSize(text_begin, text_end, false, wrap_width);
                text_bounding_box = ImRect(text_position, text_position + text_size);
            }

            ImGui::ItemSize(text_size);
            ImGui::ItemAdd(text_bounding_box, 0);

            // SET THE TEXT COLOR.
            // A temporarily different (usually) highlight color is used when hovering over.
            bool is_currently_highlighted_word = (token.Text == CurrentlyHighlightedWord) || (ImGui::IsItemHovered());
            if (is_currently_highlighted_word)
            {
                /// @todo   Something other than yellow for highlights?
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

                CurrentlyHighlightedWord = token.Text;
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            }
            // Render (we don't hide text after ## in this end-user function)
            // RenderTextWrapped didn't work, so we're rendering text directly.
            ImGui::RenderText(text_bounding_box.Min, text_begin, text_end);
            ImGui::PopStyleColor();
            // ADJUST THE COLOR FOR TESTING.
            // This is just temporary and will be replaced with fixed lookups/customizable options later.
#if RANDOM_COLORS
            if (color.x <= color.y)
            {
                color.x += 0.1f;
            }
            else if (color.y <= color.z)
            {
                color.y += 0.1f;
            }
            else if (color.z <= color.x)
            {
                color.z += 0.1f;
            }
#endif
        }
    }

    /// Updates the lookup of colors for the current set of verses.
    void BibleVersesWindow::UpdateColorLookup()
    {
        const std::unordered_set<std::string> STOP_WORDS =
        {
            "a",
            "the"
        };

        // UPDATE COLORS FOR ANY UNCOLORED WORDS.
        std::random_device random_number_generator;
        ImVec4 color = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f };
        for (const BIBLE_DATA::BibleVerse& verse : Verses)
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

                // SKIP STORING COLORS FOR OVERLY COMMON WORDS.
                bool is_stop_word = STOP_WORDS.contains(token.Text);
                if (is_stop_word)
                {
                    continue;
                }

                // UPDATE THE CURRENT WORDS COLOR.
                /// @todo   How to preserve colors while not re-using colors?
                /// @todo   Upper/lowercase?
                auto current_word_color = ColorsByWord.find(token.Text);
                bool current_word_color_exists = (ColorsByWord.cend() != current_word_color);
                if (!current_word_color_exists)
                {
                    // ASSIGN SOME RANDOM COLORS.
                    color.x = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                    color.y = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                    color.z = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());

                    ColorsByWord[token.Text] = color;
                }
            }
        }
    }

    /// Computes statistics about the words currently displayed in the window.
    void BibleVersesWindow::ComputeWordStatistics()
    {
        OccurrenceCountsByWord.clear();

        for (const BIBLE_DATA::BibleVerse& verse : Verses)
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

                // COUNT THE WORD.
                ++OccurrenceCountsByWord[token.Text];
            }
        }
    }
}
