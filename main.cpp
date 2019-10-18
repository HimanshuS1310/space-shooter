#include <trymacro.h>

#include "engine.h"


void resizeConsole()
{
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 640, 640, TRUE);
}

mango::int32 main()
{
    M_TRY
    {
        resizeConsole();

        engine::Game::game_handle game = new engine::Game();
        game->Play();
    }
        M_CATCH("main.txt")
}

//M_TRY {} M_CATCH(main.txt) --> try catch block to catch 
//mango::exception:: and critical errors and logging them to main.txt.
