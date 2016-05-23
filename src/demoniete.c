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
#include "demoniete.h"
#include "sprites.h"

/**
 * @brief Función específica del sprite demoniete que calcula la dirección del movimento horizontal y el tiempo de espera entre cambios
 * @param *map Puntero a la estructura del mapa
 * @param *demoniete Puntero al sprite que se define como demoniete y tendrá ese determinado comportamiento
 */

void demoniete_move(Rock_Scroll_Map *map, Rock_Sprite *demoniete)
{
  int16_t check_point;

    if (!demoniete->active || demoniete->current_pause)
        return;

    if (demoniete_falls(map, demoniete))
        return;

    if (demoniete->wait > 0) {
        demoniete->wait --;
        return;
    }

    if (!demoniete->h_move) {
        if (rand() % 5) /* nos moveremos de vez en cuando */
            return;
    }

    if (!demoniete->h_move && !demoniete->current_frame)
        demoniete_change_direction(demoniete);

    check_point = demoniete->x_map + demoniete->h_move;

    if ((demoniete->h_move != 0 && !sprites_can_move(check_point, demoniete->y_map, demoniete, map, 0)) || demoniete->min_frames_move <= 0) {

        demoniete->frames = (demoniete->h_move == -1) ? &demon[WAIT_LEFT] : &demon[WAIT_RIGHT];
        demoniete->h_move = 0;
        demoniete->current_frame = 0;
        demoniete->num_frames = 4;
        demoniete->min_frames_move = (rand() % 60) + 60;
    }

    sprites_move(demoniete, map);
}

/**
 * @brief Establece cuando tiene que cambiar de dirección de manera pseudoaleatoria
 * @param *demoniente Puntero al sprite
 */

void demoniete_change_direction(Rock_Sprite *demoniete)
{
  uint8_t orientation;

    orientation = rand() % 2;

    if (!orientation) {
        demoniete->h_move = -1;
        demoniete->frames = &demon[LEFT];
        demoniete->num_frames = 12;
    } else {
        demoniete->h_move = 1;
        demoniete->frames = &demon[RIGHT];
        demoniete->num_frames = 12;
    }
}

/**
 * @brief Determina si el sprite puede y debe caer, cambiando la función de movimento por la genérica de caída
 * @param *map Puntero a la estructura del mapa
 * @param *demoniente Puntero al sprite
 */

int16_t demoniete_falls(Rock_Scroll_Map *map, Rock_Sprite *demoniete)
{
  bool falls = false;
  int16_t pos_x, pos_y;

    pos_x = demoniete->x_map;
    pos_y = demoniete->y_map;

    if (map->rockmap[pos_x][pos_y + 1] == GROUND_BONE || map->rockmap[pos_x][pos_y + 1] == GROUND)
        falls = true;

    if (map->objects_map[pos_x][pos_y + 1] != EMPTY && (pos_y + 1) <= MAXMAP_UNITS_Y)
        falls = false;

    if (map->objects_map[pos_x][pos_y + 1] == map->rocco_index) {
        falls = true;
        sprites_death(map, sprite_get_object(map->rocco_index), true);
    }

    if (falls)
        demoniete->move = sprites_falls;

    return falls;
}
