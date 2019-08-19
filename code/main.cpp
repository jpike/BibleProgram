#include <cstdlib>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ThirdParty/gl3w/GL/gl3w.h>
#include <ThirdParty/gl3w/GL/glcorearb.h>
#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_impl_opengl3.h>
#include <ThirdParty/imgui/imgui_impl_sdl.h>
#include <ThirdParty/SDL/SDL.h>
#undef main
#include "BibleData/VersePerLineFile.h"

int main()
{
    // PARSE THE BIBLE DATA.
    std::optional<BIBLE_DATA::VersePerLineFile> verse_per_line_file = BIBLE_DATA::VersePerLineFile::Parse("../data/SacredTexts/kjvdat.txt");
    if (verse_per_line_file)
    {
        std::cout << "Successful parse." << std::endl;
    }
    else
    {
        std::cerr << "Failed parse." << std::endl;
        return EXIT_FAILURE;
    }

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

        // UPDATING.
        bool show_demo_window = true;

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

            // DRAWING.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();
            ImGui::ShowDemoWindow(&show_demo_window);

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
            std::cout << "Frame Time (ms): " << (SDL_GetTicks() - elapsed_time_in_ms_until_previous_frame) << std::endl;
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
