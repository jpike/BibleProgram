#include <cstdlib>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <emscripten.h>
#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_impl_opengl3.h>
#include <ThirdParty/imgui/imgui_impl_sdl.h>
#include <ThirdParty/SDL/SDL.h>
#undef main
#include "BibleData/VersePerLineFile.h"

SDL_Window* g_window = nullptr;
SDL_GLContext g_gl_context = nullptr;

void EmscriptenMainLoopIteration()
{
    // EVENT HANDLING.
    SDL_Event application_event;
    while (SDL_PollEvent(&application_event) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&application_event);
    }

    // DRAWING.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(g_window);
    ImGui::NewFrame();

    static bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    SDL_GL_MakeCurrent(g_window, g_gl_context);

    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    const ImVec4 BACKGROUND_COLOR = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, BACKGROUND_COLOR.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(g_window);
}

int main()
{
    try
    {
        // INITIALIZE SDL.
        constexpr uint32_t SDL_SUBSYSTEMS = (SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO);
        constexpr int SDL_SUCCESS_RETURN_CODE = 0;
        SDL_Init(SDL_SUBSYSTEMS);

        constexpr int SCREEN_WIDTH_IN_PIXELS = 1280;
        constexpr int SCREEN_HEIGHT_IN_PIXELS = 768;
        constexpr SDL_WindowFlags WINDOW_SETTINGS = static_cast<SDL_WindowFlags>(SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        g_window = SDL_CreateWindow(
            "Bible Program",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH_IN_PIXELS,
            SCREEN_HEIGHT_IN_PIXELS,
            WINDOW_SETTINGS);

        g_gl_context = SDL_GL_CreateContext(g_window);
        SDL_GL_MakeCurrent(g_window, g_gl_context);

        constexpr int VERTICAL_SYNC = 1;
        SDL_GL_SetSwapInterval(VERTICAL_SYNC);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplSDL2_InitForOpenGL(g_window, g_gl_context);
        const char* const GLSL_VERSION = "#version 100";
        ImGui_ImplOpenGL3_Init(GLSL_VERSION);
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Standard exception: " << exception.what() << std::endl
            << "SDL error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr
            << "Unknown exception." << std::endl
            << "SDL error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    constexpr int LET_BROWSER_CONTROL_FRAME_RATE = 0;
    constexpr int SIMULATE_INFINITE_LOOP = 1;
    emscripten_set_main_loop(EmscriptenMainLoopIteration, LET_BROWSER_CONTROL_FRAME_RATE, SIMULATE_INFINITE_LOOP);

    return EXIT_SUCCESS;
}
