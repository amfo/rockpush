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

#include "rockpush.h"
#include "collision.h"
#include "sprites.h"

/**
 * @brief Comprueba la colisión del sprite en los dos escenarios: objetos fijos y sprites
 * @param *spr Puntero de a estructura de tipo Rock_Sprite del objeto que interactúa
 * @param *map Puntero a estructura de tipo Rock_Scroll_Map
 * @param *collision_object Puntero a estructura de tipo Collision_Object que tedermina la configuración de los parámetros de colisión
 */

void collision_sprite(Rock_Sprite *spr, Rock_Scroll_Map *map, Collision_Object *collision_object)
{
  int16_t x, y;
  int16_t x1, y1, x2, y2; /* posiciones */
  double distance = 0, collision_distance = 0;
  Rock_Sprite *object;
  Perimeter perimeter;

    collision_set_perimeter(spr, map, &perimeter, COLLISION);
    collision_object->collision = false;
    collision_object->value_object_map = -1;
    collision_object->object = NULL;

    for (y = perimeter.from_y; y < perimeter.to_y; y ++) {

        for (x = perimeter.from_x; x < perimeter.to_x; x ++) {

            if (
                collision_object->object_type == OBJECT_SPRITE &&
                map->objects_map[x][y] > EMPTY && map->objects_map[x][y] != spr->index
            ) {

                object = sprite_get_object(map->objects_map[x][y]);

                x2 = object->x_map * TILE_SIZE + object->offset_x + TILE_HALF;
                y2 = object->y_map * TILE_SIZE + object->offset_y + TILE_HALF;
                x1 = spr->x_map * TILE_SIZE + spr->offset_x + TILE_HALF;
                y1 = spr->y_map * TILE_SIZE + spr->offset_y + TILE_HALF;

                distance           = pow((x2 - x1), 2) + pow((y2 - y1), 2);
                collision_distance = pow((spr->collision_radius + object->collision_radius), 2);

                if (distance < collision_distance) {
                    collision_object->collision = true;
                    collision_object->value_object_map = object->type_object;
                    collision_object->object = object;
                } else if (distance < TILE_DISTANCE && map->objects_map[spr->x_map + spr->h_move][spr->y_map + spr->v_move] > EMPTY) {
                    collision_object->collision = true;
                    collision_object->value_object_map = object->type_object;
                    collision_object->object = object;
                }

            }

            if (
                collision_object->object_type == OBJECT_STATIC &&
                map->rockmap[x][y] != GROUND_BONE &&
                map->rockmap[x][y] != GROUND &&
                map->rockmap[x][y] != collision_object->ignore_object
            ) {

                x2 = x * TILE_SIZE + TILE_HALF;
                y2 = y * TILE_SIZE + TILE_HALF;
                x1 = spr->x_map * TILE_SIZE + spr->offset_x + TILE_HALF;
                y1 = spr->y_map * TILE_SIZE + spr->offset_y + TILE_HALF;

                distance           = pow((x2 - x1), 2) + pow((y2 - y1), 2);
                collision_distance = pow((spr->collision_radius + TILE_HALF), 2);

                if (distance <= collision_distance) {
                    collision_object->collision = true;
                    collision_object->value_object_map = map->rockmap[x][y];
                    collision_object->object = NULL;
                }
            }

        }

    }
}

/**
 * @brief Determina si en las coordenadas dadas del mapa existe un objeto en colisión
 * @param *map Puntero a la estructura del mapa
 * @param pos_x Posición X en el mapa
 * @param pos_y Posición Y en el mapa
 * @param orientation La dirección hacia donde se debe comprobar la colisión
 * @return Devuelve si ha colisionado con un objeto fijo del mapa o con un sprite móvil
 */

uint8_t collision_map(Rock_Scroll_Map *map, uint8_t pos_x, uint8_t pos_y, uint8_t orientation)
{
  uint8_t object_map;
  int16_t object_sprite;

    switch (orientation) {
        case LEFT:
            object_map    = map->rockmap[pos_x - 1][pos_y];
            object_sprite = map->objects_map[pos_x - 1][pos_y];
            break;

        case RIGHT:
            object_map    = map->rockmap[pos_x + 1][pos_y];
            object_sprite = map->objects_map[pos_x + 1][pos_y];
            break;

        case UP:
            object_map    = map->rockmap[pos_x][pos_y - 1];
            object_sprite = map->objects_map[pos_x][pos_y - 1];
            break;

        case DOWN:
            object_map    = map->rockmap[pos_x][pos_y + 1];
            object_sprite = map->objects_map[pos_x][pos_y + 1];
            break;
    }

    if (object_map != GROUND && object_map != GROUND_BONE)
        return OBJECT_STATIC;

    if (object_sprite != EMPTY)
        return OBJECT_SPRITE;

    return 0;
}

/**
 * @brief Calcula si los márgenes de comprobación de colisión están dentro de los parámetros válidos
 * @param *spr Puntero al objeto que se quiere comprobar la colisión
 * @param *map Puntero a la estructura del mapa
 * @param *perimeter Puntero a estructura de tipo Perimeter donde se alamacenarán los datos del perímetro de colisión
 * @param range Radio del perímetro de colisión en posiciones de mapa
 */

void collision_set_perimeter(Rock_Sprite *spr, Rock_Scroll_Map *map, Perimeter *perimeter, int16_t range)
{
    perimeter->from_x = perimeter->to_x = perimeter->from_y = perimeter->to_y = range;

    if (spr->h_move == -1) {
        perimeter->from_x = spr->x_map - perimeter->from_x;
        perimeter->from_y = spr->y_map - perimeter->from_y;
        perimeter->to_x   = spr->x_map;
        perimeter->to_y   = spr->y_map + perimeter->to_y;
    } else if (spr->h_move == 1) {
        perimeter->from_x = spr->x_map + 1;
        perimeter->from_y = spr->y_map - perimeter->from_y;
        perimeter->to_x   = spr->x_map + perimeter->to_x + 1;
        perimeter->to_y   = spr->y_map + perimeter->to_y;
    } else if (spr->v_move == -1) {
        perimeter->from_x = spr->x_map - perimeter->to_x;
        perimeter->from_y = spr->y_map - perimeter->from_y;
        perimeter->to_x   = spr->x_map + perimeter->to_x;
        perimeter->to_y   = spr->y_map;
    } else if (spr->v_move == 1) {
        perimeter->from_x = spr->x_map - perimeter->to_x;
        perimeter->from_y = spr->y_map + 1;
        perimeter->to_x   = spr->x_map + perimeter->to_x;
        perimeter->to_y   = spr->y_map + perimeter->to_y + 1;
    }

    if (perimeter->from_x < 0)
        perimeter->from_x = 0;

    if (perimeter->to_x >= MAXMAP_UNITS_X)
        perimeter->to_x = MAXMAP_UNITS_X;

    if (perimeter->from_y < 0)
        perimeter->from_y = 0;

    if (perimeter->to_y >= MAXMAP_UNITS_Y)
        perimeter->to_y = MAXMAP_UNITS_Y;
}