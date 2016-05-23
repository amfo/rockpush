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

#include <stdlib.h>
#include "fly.h"
#include "sfx.h"

/**
 * @brief Función específica del sprite fly que determina los movimientos en sentido de las agujas del reloj con diferentes orientaciones
 * @param *map Puntero a la estructura del mapa
 * @param *object_fly Puntero al sprite que se define como fly tendrá ese determinado comportamiento
 */

void fly_move(Rock_Scroll_Map *map, Rock_Sprite *object_fly)
{
    int16_t orientation, view, move_x, move_y;

    if (!object_fly->active || object_fly->current_pause)
        return;

    if (object_fly->wait > 0) {
        object_fly->wait --;
        return;
    }

    /* Inicializamos hacia dónde irá el bicho */
    if (!object_fly->h_move && !object_fly->v_move) {
        orientation = (rand() % 4) + 2;
        view = rand() % 2;
        view = (view) ? RIGHT : LEFT;
        fly_change_orientation(object_fly, orientation);
    }

    move_x = object_fly->x_map + object_fly->h_move;
    move_y = object_fly->y_map + object_fly->v_move;

    if (!sprites_can_move(move_x, move_y, object_fly, map, 0)) {
        fly_change_direction(object_fly);
    } else
        sprites_move(object_fly, map);

}

/**
 * @brief Determina si cambia de dirección
 * @param *object_fly Puntero al sprite
 */

void fly_change_direction(Rock_Sprite *object_fly)
{
    if (object_fly->h_move == -1)
        fly_change_orientation(object_fly, DOWN);
    else if (object_fly->v_move == 1)
        fly_change_orientation(object_fly, RIGHT);
    else if (object_fly->h_move == 1)
        fly_change_orientation(object_fly, UP);
    else if (object_fly->v_move == -1)
        fly_change_orientation(object_fly, LEFT);
}

/**
 * @brief Establece el cambio de dirección
 * @param *object_fly Puntero al sprite
 * @param orientation Hacian dónde mirará. Si el movimiento es vertical mirará de manera aleatoria
 */

void fly_change_orientation(Rock_Sprite *object_fly, int16_t orientation)
{
  int16_t view;

    view = rand() % 2;
    view = (view) ? RIGHT : LEFT;

    switch (orientation) {
        case RIGHT:
            object_fly->h_move = 1;
            object_fly->v_move = 0;
            object_fly->frames = &fly[RIGHT];
            break;

        case LEFT:
            object_fly->h_move = -1;
            object_fly->v_move = 0;
            object_fly->frames = &fly[LEFT];
            break;

        case DOWN:
            object_fly->h_move = 0;
            object_fly->v_move = 1;
            object_fly->frames = &fly[view];
            break;

        case UP:
            object_fly->h_move = 0;
            object_fly->v_move = -1;
            object_fly->frames = &fly[view];
            break;

        default:
            object_fly->h_move = 1;
            object_fly->v_move = 0;
            object_fly->frames = &fly[RIGHT];
            break;
    }
}

/**
 * @brief Transforma al sprite fly en diamantes cuando le cae un objeto encima
 * @param *map Puntero al mapa
 * @param *object_fly Puntero al sprite
 */

void fly_to_diamonds(Rock_Scroll_Map *map, Rock_Sprite *object_fly)
{
  int16_t from_x, from_y, to_x, to_y, x, y, index, new_diamonds;
  Rock_Sprite *spr;

  struct new_coordinates {
      int16_t x;
      int16_t y;
      int16_t index;
  } new_coordinates[9];

    new_diamonds = -1;

    from_x = (object_fly->x_map - 1) > 0 ? object_fly->x_map - 1 : 0;
    to_x   = object_fly->x_map + 1;
    from_y = (object_fly->y_map - 1 > 0) ? object_fly->y_map - 1 : 0;
    to_y   = object_fly->y_map + 1;
    index  = map->objects_map[object_fly->x_map][object_fly->y_map];

    if (object_fly->is_visible)
        sfx_play(SFX_DIAMOND);

    object_fly->is_visible = false;

    for (y = from_y; y <= to_y && y < MAXMAP_UNITS_Y; y ++) {
        for (x = from_x; x <= to_x && x < MAXMAP_UNITS_X; x++) {
            if (map->rockmap[x][y] != GROUND &&  map->rockmap[x][y] != GROUND_BONE)
                map->rockmap[x][y] = GROUND;

            if (map->objects_map[x][y] > EMPTY && map->objects_map[x][y] != index) {
                spr = sprite_get_object(map->objects_map[x][y]);

                switch (spr->type_object) {
                    /*case FLY:
                        fly_to_diamonds(map, spr);
                        break;*/

                    case ROCCO:
                        sprites_death(map, spr, true);
                        break;

                    case DIAMOND:
                        break;

                    default:
                        sprites_define_object(spr, spr->x_map, spr->y_map, DIAMOND);
                        spr->index = map->objects_map[x][y];
                        break;
                }

            } else if (map->objects_map[x][y] < 0 && map->objects_map[x][y] != index) {
                new_diamonds ++;
                new_coordinates[new_diamonds].x = x;
                new_coordinates[new_diamonds].y = y;
                new_coordinates[new_diamonds].index = map->objects + new_diamonds;
            }

        }
    }

    sprites_define_object(object_fly, object_fly->x_map, object_fly->y_map, DIAMOND);
    object_fly->index = index;
    map->objects_map[object_fly->x_map][object_fly->y_map] = index;

    if (new_diamonds >= 0) {
        new_diamonds ++;
        map->objects += new_diamonds;
        sprites_add(map->objects);

        for (index = 0; index < new_diamonds; index ++) {
            spr = sprite_get_object(new_coordinates[index].index);
            sprites_define_object(spr, new_coordinates[index].x, new_coordinates[index].y, DIAMOND);
            spr->index = map->objects_map[ new_coordinates[index].x][new_coordinates[index].y] = new_coordinates[index].index;
        }

        map->refresh_rocco = true;
    }
}