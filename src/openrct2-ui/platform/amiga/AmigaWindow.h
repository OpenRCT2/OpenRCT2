/*****************************************************************************
 * The SDL_Window of the AmigaOS shim: an 8-bit RTG screen with a backdrop window.
 *****************************************************************************/
#pragma once

#include <string>

struct SDL_Window
{
    int width = 0;
    int height = 0;
    std::string title;
    bool fullscreen = true;
};
