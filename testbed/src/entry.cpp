#include "game.h"
#include <entry.h>
//TODO: remove this
#include<platform.h>
// استدعاء دالة الطباعة من المكتبة

b8 create_game(game *out_game) {
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 800;
    out_game->app_config.start_height = 720;
    out_game->app_config.name = "kohi engine testbed";
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->initialize = game_initialize;
    out_game->one_resized = game_one_resized;
    out_game->state = platform_allocate(sizeof(game_state), false);

    return true;
}
