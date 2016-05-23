/* Copyright (c) 2015-2016, Antonio M. Fernández Orts <antonio.fernandez at gmx dot com>
 *
 * This file is part of RockPush.
 *
 * RockPush is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RockPush is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RockPush.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#include "guzzler.h"
#include "dummy.h"
#include "sprites.h"
#include "sfx.h"

/**
 * @brief Función específica del sprite guzzler que determina los movimientos con un rango aleatorio
 * @param *map Puntero a la estructura del mapa
 * @param *object_guzzler Puntero al sprite que se define como guzzler tendrá ese determinado comportamiento
 */

void guzzler_move(Rock_Scroll_Map *map, Rock_Sprite *object_guzzler)
{
  int16_t move_x, move_y;

    if (!object_guzzler->active || object_guzzler->current_pause)
        return;

    if (object_guzzler->wait > 0) {
        object_guzzler->wait --;
        return;
    }

    if (object_guzzler->min_frames_move <= 0)
        guzzler_change_direction(object_guzzler);

    move_x = object_guzzler->x_map + object_guzzler->h_move;
    move_y = object_guzzler->y_map + object_guzzler->v_move;

    if (!sprites_can_move(move_x, move_y, object_guzzler, map, GRASS)) {
        guzzler_change_direction(object_guzzler);
    } else {
        sprites_move(object_guzzler, map);

        if (map->rockmap[object_guzzler->x_map][object_guzzler->y_map] == GRASS) {
            if (object_guzzler->is_visible)
                sfx_play(SFX_GRASS);

            object_guzzler->is_visible = false;
            map->rockmap[object_guzzler->x_map][object_guzzler->y_map] = GROUND;
        }
    }

}

/**
 * @brief Establece el cambio de dirección
 * @param *object_guzzler Puntero al sprite
 */

void guzzler_change_direction(Rock_Sprite *object_guzzler)
{
  int16_t orientation;

    orientation = rand() % 4;
    object_guzzler->min_frames_move = (rand() % 20) + 10;

    switch (orientation) {
        case 0:
            object_guzzler->h_move = -1;
            object_guzzler->v_move = 0;
            object_guzzler->frames = &guzzler[LEFT];
            break;

        case 1:
            object_guzzler->h_move = 1;
            object_guzzler->v_move = 0;
            object_guzzler->frames = &guzzler[RIGHT];
            break;

        case 2:
            object_guzzler->h_move = 0;
            object_guzzler->v_move = -1;
            object_guzzler->frames = &guzzler[UP];
            break;

        case 3:
            object_guzzler->h_move = 0;
            object_guzzler->v_move = 1;
            object_guzzler->frames = &guzzler[DOWN];
            break;

        default:
            object_guzzler->h_move = -1;
            object_guzzler->v_move = 0;
            object_guzzler->frames = &guzzler[LEFT];
            break;
    }
}
