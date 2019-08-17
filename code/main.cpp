#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
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
        std::cout << "Failed parse." << std::endl;
    }

    // INITIALIZATION.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize!  SDL_Error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    constexpr int SCREEN_WIDTH = 640;
    constexpr int SCREEN_HEIGHT = 480;
    SDL_Window* window = SDL_CreateWindow(
        "Bible Program",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Window could not be created!  SDL_Error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Surface* screen = SDL_GetWindowSurface(window);

    // UPDATING.
    uint32_t milliseconds_since_start = SDL_GetTicks();
    uint32_t elapsed_time_until_previous_frame = milliseconds_since_start;
    bool quit = false;
    while (!quit)
    {
        // EVENT HANDLING.
        SDL_Event application_event;
        while (SDL_PollEvent(&application_event) != 0)
        {
            if (SDL_QUIT == application_event.type)
            {
                quit = true;
            }
        }

        // DRAWING.
        SDL_FillRect(
            screen,
            nullptr,
            SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
        SDL_UpdateWindowSurface(window);

        constexpr float FRAMES_PER_SECOND = 60.0f;
        constexpr float SECONDS_PER_FRAME = 1.0f / FRAMES_PER_SECOND;
        constexpr float MILLISECONDS_PER_SECOND = 1000.0f;
        constexpr uint32_t MILLISECONDS_PER_FRAME = static_cast<uint32_t>(SECONDS_PER_FRAME * MILLISECONDS_PER_SECOND);
        uint32_t current_total_milliseconds = SDL_GetTicks();
        uint32_t elapsed_time_for_current_frame = (current_total_milliseconds - elapsed_time_until_previous_frame);
        if (elapsed_time_for_current_frame < MILLISECONDS_PER_FRAME)
        {
            uint32_t remaining_time_for_frame = MILLISECONDS_PER_FRAME - elapsed_time_for_current_frame;
            SDL_Delay(remaining_time_for_frame);
        }
        std::cout << "Frame Time (ms): " << (SDL_GetTicks() - elapsed_time_until_previous_frame) << std::endl;

        elapsed_time_until_previous_frame = SDL_GetTicks();
    }

    // SHUTDOWN.
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
