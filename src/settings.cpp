#include "settings.hpp"
#include "game_state.hpp"

#include <cstdlib>

SettingsCtx* settings_start(GenericCtx *generic_ctx)
{
    SettingsCtx *ctx = (SettingsCtx*)calloc(1, sizeof(SettingsCtx));

    return ctx;
}

GameReturnCode settings_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SettingsCtx *settings_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.backspace)
    {
	return_code = GameReturnCode::goto_menu;
    }

    return return_code;
}

void settings_end(SettingsCtx *settings_ctx)
{
    free(settings_ctx);
}

Settings settings_from_file(const char *path)
{
    Settings settings;

    return settings;
}
