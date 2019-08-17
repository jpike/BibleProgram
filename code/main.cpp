#include <cstdlib>
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

        SDL_Delay(100);
    }

    // SHUTDOWN.
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
