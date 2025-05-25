#include "game.h"
#include <core/logger.h>


b8 game_initialize(game *game_inst) {
    KDEBUG("game initialized() called");
    return true;
}

b8 game_update(game *game_inst, f32 delta_time) {

    return true;
}

b8 game_render(game *game_inst, f32 delta_time) {
    return true;
}

void game_one_resized(game *game_inst, i32 width, i32 height) {
}
