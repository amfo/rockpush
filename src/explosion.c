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

#include "explosion.h"
#include "sprites.h"
#include "sfx.h"

/**
 * @brief Se inicializa la explosión, eliminando en un radio determinado todo objeto en él. Si hay otra bomba se hará una llamada recursiva
 * @param *map Puntero a estrucutra de tipo Rock_Scroll_Map donde se encuentra la información del mapa y los objetos que hay en él
 * @param *bomb Puntero al sprite de la bomba que causa la explosión
 */

void explosion_set(Rock_Scroll_Map *map, Rock_Sprite *bomb)
{
  int16_t from_x, from_y, to_x, to_y, x, y, index;
  Rock_Sprite *spr;

    from_x = (bomb->x_map - 1) > 0 ? bomb->x_map - 1 : 0;
    to_x   = bomb->x_map + 1;
    from_y = (bomb->y_map - 1 > 0) ? bomb->y_map - 1 : 0;
    to_y   = bomb->y_map + 1;
    index  = map->objects_map[bomb->x_map][bomb->y_map];

    if (bomb->is_visible)
        sfx_play(SFX_EXPLOSION);

    bomb->is_visible = false;

    for (y = from_y; y <= to_y && y < MAXMAP_UNITS_Y; y ++) {
        for (x = from_x; x <= to_x && x < MAXMAP_UNITS_X; x++) {
            if (map->rockmap[x][y] != GROUND &&  map->rockmap[x][y] != GROUND_BONE)
                map->rockmap[x][y] = GROUND;

            if (map->objects_map[x][y] > EMPTY && map->objects_map[x][y] != index) {
                spr = sprite_get_object(map->objects_map[x][y]);
                if (spr->type_object == BOMB)
                    explosion_set(map, spr);
                else if (spr->type_object == ROCCO) {
                    spr->active = false;
                    map->objects_map[x][y] = EMPTY;
                } else
                    map->objects_map[x][y] = EMPTY;

            } else
                map->objects_map[x][y] = EMPTY;


        }
    }

    // Transforma el sprite bomba en sprite explosión y efectuar la animación correspondiente
    sprites_define_object(bomb, bomb->x_map, bomb->y_map, EXPLOSION);
    map->objects_map[bomb->x_map][bomb->y_map] = index;
    bomb->index = index;
}

/**
 * @brief Cuando termina la animación de la explosión se comprueba si Rocco se encuentra en el perímetro y elimina el objeto de la explosión
 * @param *map Puntero al mapa
 * @param *explosion Puntero al sprite explosion que previamente era el sprite bomb
 */

void explosion_end(Rock_Scroll_Map *map, Rock_Sprite *explosion)
{
  Rock_Sprite *rocco;

    if (sprites_destroy(map, explosion)) {
        rocco = sprite_get_object(map->rocco_index);

        if (!rocco->active && map->objects_map[rocco->x_map][rocco->y_map] == EMPTY) {
            sprites_define_object(rocco, rocco->x_map, rocco->y_map, ROCCO_BURNED);
            map->objects_map[rocco->x_map][rocco->y_map] = map->rocco_index;
        }

        map->objects_map[explosion->x_map][explosion->y_map] = EMPTY;
    }
}