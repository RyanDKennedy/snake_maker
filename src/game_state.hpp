#pragma once


enum class GameState
{
    menu,
    snake,
    scoreboard,
    settings,
    map_create,
};

enum class GameReturnCode
{
    none,
    play_snake,
    goto_menu,
    goto_settings,
};
