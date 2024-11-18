#pragma once


enum class GameState
{
    menu,
    snake,
    scoreboard,
};

enum class GameReturnCode
{
    none,
    play_snake,
    goto_menu,
};
