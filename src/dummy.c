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
#include "dummy.h"

/**
 * @brief Función específica del sprite dummy que determina los movimientos en sentido de las agujas del reloj
 * @param *map Puntero a la estructura del mapa
 * @param *dummy Puntero al sprite que se define como dummy y tendrá ese determinado comportamiento
 */

void dummy_move(Rock_Scroll_Map *map, Rock_Sprite *dummy)
{
  int16_t orientation, move_x, move_y;

    if (!dummy->active || dummy->current_pause)
        return;

    if (dummy->wait > 0) {
        dummy->wait --;
        return;
    }

    /* Inicializamos hacia dónde irá el bicho */
    if (!dummy->h_move && !dummy->v_move) {
        orientation = rand() % 4;

        switch (orientation) {
            case 0:
                dummy->h_move = -1;
                dummy->v_move = 0;
                break;

            case 1:
                dummy->h_move = 1;
                dummy->v_move = 0;
                break;

            case 2:
                dummy->h_move = 0;
                dummy->v_move = -1;
                break;

            case 3:
                dummy->h_move = 0;
                dummy->v_move = 1;
                break;

            default:
                dummy->h_move = -1;
                dummy->v_move = 0;
                break;
        }
    }

    move_x = dummy->x_map + dummy->h_move;
    move_y = dummy->y_map + dummy->v_move;

    if (!sprites_can_move(move_x, move_y, dummy, map, 0)) {
        dummy_change_direction(dummy);
    } else
        sprites_move(dummy, map);

}

/**
 * @brief Establece el cambio de dirección
 * @param *dummy Puntero al sprite
 */

void dummy_change_direction(Rock_Sprite *dummy)
{
    if (dummy->h_move == -1) {
        dummy->h_move = 0;
        dummy->v_move = -1;
    } else if (dummy->v_move == -1) {
        dummy->h_move = 1;
        dummy->v_move = 0;
    } else if (dummy->h_move == 1) {
        dummy->h_move = 0;
        dummy->v_move = 1;
    } else if (dummy->v_move == 1) {
        dummy->h_move = -1;
        dummy->v_move = 0;
    }

}
