#include <cassert>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <variant>
#include <ThirdParty/gl3w/GL/gl3w.h>
#include <ThirdParty/gl3w/GL/glcorearb.h>
#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_impl_opengl3.h>
#include <ThirdParty/imgui/imgui_impl_sdl.h>
#include <ThirdParty/SDL/SDL.h>
#undef main
#include "BibleData/Bible.h"
#include "BibleData/BibleTranslation.h"
#include "BibleData/BibleVerseRange.h"
#include "BibleData/Files/BibleDataFiles.h"
#include "Debugging/Timer.h"
#include "Gui/Gui.h"

int main2()
{
#if SERIAL
    using ClockType = std::chrono::high_resolution_clock;

    DEBUGGING::Timer<ClockType> main_timer("Main"); // 4353ms
    {
        DEBUGGING::Timer<ClockType> kjv_timer("KJV load"); // 1357ms
        auto kjv = BIBLE_DATA::FILES::BibleDataFile::ParseVersePerLineFile("KJV", "data/SacredTexts/kjvdat.txt");
    }

    {
        DEBUGGING::Timer<ClockType> bbe_timer("BBE load"); // 1001ms
        auto bbe = BIBLE_DATA::FILES::BibleDataFile::ParseOsisXmlFile("BBE", "data/GratisBible/bbe.xml");
    }

    {
        DEBUGGING::Timer<ClockType> web_timer("WEB load"); // 993ms
        auto web = BIBLE_DATA::FILES::BibleDataFile::ParseOsisXmlFile("WEB", "data/GratisBible/web.xml");
    }

    {
        DEBUGGING::Timer<ClockType> ylt_timer("YLT load"); // 1001ms
        auto ylt = BIBLE_DATA::FILES::BibleDataFile::ParseOsisXmlFile("YLT", "data/GratisBible/ylt.xml");
    }
#endif

    using ClockType = std::chrono::high_resolution_clock;

    DEBUGGING::Timer main_timer("Main"); // 3021ms

    auto kjv_timer = std::make_unique<DEBUGGING::Timer<ClockType>>("KJV load"); // 2715ms
    auto kjv_bible_loading = std::async(std::launch::async, [] { return BIBLE_DATA::FILES::BibleDataFile::ParseVersePerLineFile("KJV", "data/SacredTexts/kjvdat.txt"); });

    auto bbe_timer = std::make_unique<DEBUGGING::Timer<ClockType>>("BBE load"); // 2715ms
    auto bbe_bible_loading = std::async(std::launch::async, [] { return BIBLE_DATA::FILES::BibleDataFile::ParseOsisXmlFile("BBE", "data/GratisBible/bbe.xml"); });

    auto web_timer = std::make_unique<DEBUGGING::Timer<ClockType>>("WEB load"); // 2715ms
    auto web_bible_loading = std::async(std::launch::async, [] { return BIBLE_DATA::FILES::BibleDataFile::ParseOsisXmlFile("WEB", "data/GratisBible/web.xml"); });

    auto ylt_timer = std::make_unique<DEBUGGING::Timer<ClockType>>("YLT load"); // 2715ms
    auto ylt_bible_loading = std::async(std::launch::async, [] { return BIBLE_DATA::FILES::BibleDataFile::ParseOsisXmlFile("YLT", "data/GratisBible/ylt.xml"); });
    
    auto kjv = kjv_bible_loading.get();
    kjv_timer.reset();

    auto bbe = bbe_bible_loading.get();
    bbe_timer.reset();

    auto web = web_bible_loading.get();
    web_timer.reset();

    auto ylt = ylt_bible_loading.get();
    ylt_timer.reset();

    return 0;
}

int main_tests()
{
    std::optional<BIBLE_DATA::BibleVerseRange> verse_range;

    verse_range = BIBLE_DATA::BibleVerseRange::Parse("Hebrews 1");
#define ASSERTS 1
#if ASSERTS
    assert(verse_range);
    assert(verse_range->StartingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->StartingVerse.ChapterNumber == 1);
    assert(verse_range->StartingVerse.VerseNumber == 1);
    assert(verse_range->EndingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->EndingVerse.ChapterNumber == 1);
    assert(verse_range->EndingVerse.VerseNumber == 14);
#endif

    verse_range = BIBLE_DATA::BibleVerseRange::Parse("Hebrews 1:2");
#if ASSERTS
    assert(verse_range);
    assert(verse_range->StartingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->StartingVerse.ChapterNumber == 1);
    assert(verse_range->StartingVerse.VerseNumber == 2);
    assert(verse_range->EndingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->EndingVerse.ChapterNumber == 1);
    assert(verse_range->EndingVerse.VerseNumber == 2);
#endif

    verse_range = BIBLE_DATA::BibleVerseRange::Parse("Hebrews 1-2");
#if ASSERTS
    assert(verse_range);
    assert(verse_range->StartingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->StartingVerse.ChapterNumber == 1);
    assert(verse_range->StartingVerse.VerseNumber == 1);
    assert(verse_range->EndingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->EndingVerse.ChapterNumber == 2);
    assert(verse_range->EndingVerse.VerseNumber == 18);
#endif

    verse_range = BIBLE_DATA::BibleVerseRange::Parse("Hebrews 1:2-Hebrews 1:6");
#if ASSERTS
    assert(verse_range);
    assert(verse_range->StartingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->StartingVerse.ChapterNumber == 1);
    assert(verse_range->StartingVerse.VerseNumber == 2);
    assert(verse_range->EndingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->EndingVerse.ChapterNumber == 1);
    assert(verse_range->EndingVerse.VerseNumber == 6);
#endif

    verse_range = BIBLE_DATA::BibleVerseRange::Parse("Hebrews 1:2 - Hebrews 2:6");
#if ASSERTS
    assert(verse_range);
    assert(verse_range->StartingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->StartingVerse.ChapterNumber == 1);
    assert(verse_range->StartingVerse.VerseNumber == 2);
    assert(verse_range->EndingVerse.Book == BIBLE_DATA::BibleBookId::HEBREWS);
    assert(verse_range->EndingVerse.ChapterNumber == 2);
    assert(verse_range->EndingVerse.VerseNumber == 6);
#endif

    verse_range = BIBLE_DATA::BibleVerseRange::Parse("1 Samuel 10");
#if ASSERTS
    assert(verse_range);
    assert(verse_range->StartingVerse.Book == BIBLE_DATA::BibleBookId::FIRST_SAMUEL);
    assert(verse_range->StartingVerse.ChapterNumber == 10);
    assert(verse_range->StartingVerse.VerseNumber == 1);
    assert(verse_range->EndingVerse.Book == BIBLE_DATA::BibleBookId::FIRST_SAMUEL);
    assert(verse_range->EndingVerse.ChapterNumber == 10);
    assert(verse_range->EndingVerse.VerseNumber == 27);
#endif

    verse_range = BIBLE_DATA::BibleVerseRange::Parse("1 Samuel 1:2 - 1 Samuel 2:3");
#if ASSERTS
    assert(verse_range);
    assert(verse_range->StartingVerse.Book == BIBLE_DATA::BibleBookId::FIRST_SAMUEL);
    assert(verse_range->StartingVerse.ChapterNumber == 1);
    assert(verse_range->StartingVerse.VerseNumber == 2);
    assert(verse_range->EndingVerse.Book == BIBLE_DATA::BibleBookId::FIRST_SAMUEL);
    assert(verse_range->EndingVerse.ChapterNumber == 2);
    assert(verse_range->EndingVerse.VerseNumber == 3);
#endif

    return 0;
}

int main()
{
    // START LOADING THE BIBLE DATA.
    auto index_start_time = std::chrono::system_clock::now();
    BIBLE_DATA::FILES::BibleDataFiles bible_data_files = std::move(BIBLE_DATA::FILES::BibleDataFiles::StartLoading());
    auto index_end_time = std::chrono::system_clock::now();
    auto load_time_diff = index_end_time - index_start_time;
    std::cout << "Initial bible data files load time: " << load_time_diff.count() << std::endl;
    std::cout << "Initial bible data files load time (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(load_time_diff).count() << std::endl;
    std::cout << "Initial bible data files load time (s): " << std::chrono::duration_cast<std::chrono::seconds>(load_time_diff).count() << std::endl;

#if 0
    // 6 seconds, 330 MB memory usage with this added in.
    auto index_start_time = std::chrono::system_clock::now();
    bible->BuildWordIndex();
    auto index_end_time = std::chrono::system_clock::now();
    auto load_time_diff = index_end_time - index_start_time;
    std::cout << "Index load time: " << load_time_diff.count() << std::endl;
    std::cout << "Index load time (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(load_time_diff).count() << std::endl;
    std::cout << "Index load time (s): " << std::chrono::duration_cast<std::chrono::seconds>(load_time_diff).count() << std::endl;
#endif

    // CATCH ANY EXCEPTIONS.
    // A lot of things like SDL functions can easily fail.  To easily catch generic errors, everything's wrapped
    // in a try block, although it's not clear yet if that will sufficiently catch all errors.
    // If no errors occurred, success is assumed.
    int exit_code = EXIT_SUCCESS;
    // A few variables need to be exposed for shutdown.
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;
    try
    {
        // INITIALIZE SDL.
        constexpr uint32_t SDL_SUBSYSTEMS = (SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO);
        constexpr int SDL_SUCCESS_RETURN_CODE = 0;
        SDL_Init(SDL_SUBSYSTEMS);

        // CREATE THE WINDOW.
        constexpr int SCREEN_WIDTH_IN_PIXELS = 1280;
        constexpr int SCREEN_HEIGHT_IN_PIXELS = 768;
        constexpr SDL_WindowFlags WINDOW_SETTINGS = static_cast<SDL_WindowFlags>(SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        window = SDL_CreateWindow(
            "Bible Program",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH_IN_PIXELS,
            SCREEN_HEIGHT_IN_PIXELS,
            WINDOW_SETTINGS);

        // INITIALIZE OPEN GL FOR SDL.
        gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);
        constexpr int VERTICAL_SYNC = 1;
        SDL_GL_SetSwapInterval(VERTICAL_SYNC);

        // LOAD OPEN GL.
        gl3wInit();

        // INITIALIZE IM GUI.
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        const char* const GLSL_VERSION = "#version 130";
        ImGui_ImplOpenGL3_Init(GLSL_VERSION);


        auto bible = std::make_shared<BIBLE_DATA::Bible>();

#if 0
        GUI::Gui gui
        { 
            .TranslationConfig = GUI::BibleTranslationConfig::Create(*bible),
            .Bible = *bible 
        };
#else
        GUI::Gui gui;
        gui.Bible = bible;
        gui.BibleVersesWindow.Bible = bible;
#endif       

        // UPDATING.
        uint32_t elapsed_time_in_ms_until_previous_frame = SDL_GetTicks();
        bool quit = false;
        while (!quit)
        {
            // EVENT HANDLING.
            SDL_Event application_event;
            while (SDL_PollEvent(&application_event) != 0)
            {
                ImGui_ImplSDL2_ProcessEvent(&application_event);
                if (SDL_QUIT == application_event.type)
                {
                    quit = true;
                }
            }

            // UPDATE THE BIBLE DATA IF NEW DATA HAS BEEN LOADED.
            std::shared_ptr<BIBLE_DATA::BibleTranslation> next_bible_data_file = bible_data_files.GetNextLoadedFile();
            if (next_bible_data_file)
            {
                bible->TranslationsByName[next_bible_data_file->Name] = next_bible_data_file;
            }

            // DRAWING.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            gui.UpdateAndRender();

            ImGui::Render();
            ImGuiIO& io = ImGui::GetIO();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            const ImVec4 BACKGROUND_COLOR = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, BACKGROUND_COLOR.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);

            // ENFORCE A FRAME RATE.
            constexpr float FRAMES_PER_SECOND = 60.0f;
            constexpr float SECONDS_PER_FRAME = 1.0f / FRAMES_PER_SECOND;
            constexpr float MILLISECONDS_PER_SECOND = 1000.0f;
            constexpr uint32_t MILLISECONDS_PER_FRAME = static_cast<uint32_t>(SECONDS_PER_FRAME * MILLISECONDS_PER_SECOND);
            uint32_t current_total_milliseconds = SDL_GetTicks();
            uint32_t elapsed_time_in_ms_for_current_frame = (current_total_milliseconds - elapsed_time_in_ms_until_previous_frame);
            if (elapsed_time_in_ms_for_current_frame < MILLISECONDS_PER_FRAME)
            {
                uint32_t remaining_time_in_ms_for_frame = MILLISECONDS_PER_FRAME - elapsed_time_in_ms_for_current_frame;
                SDL_Delay(remaining_time_in_ms_for_frame);
            }
            //std::cout << "Frame Time (ms): " << (SDL_GetTicks() - elapsed_time_in_ms_until_previous_frame) << std::endl;
            elapsed_time_in_ms_until_previous_frame = SDL_GetTicks();
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Standard exception: " << exception.what() << std::endl
            << "SDL error: " << SDL_GetError() << std::endl;
        exit_code = EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr
            << "Unknown exception." << std::endl
            << "SDL error: " << SDL_GetError() << std::endl;
        exit_code = EXIT_FAILURE;
    }

    // SHUTDOWN.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (gl_context)
    {
        SDL_GL_DeleteContext(gl_context);
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
    return exit_code;
}
