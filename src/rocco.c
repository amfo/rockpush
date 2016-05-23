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

#include <stdbool.h>
#include "rocco.h"
#include "map.h"
#include "sprites.h"
#include "sfx.h"

/**
 * @brief Determina las posiciones iniciales del personaje principal
 * @param level Nivel del mapa
 * @param *x Puntero a la variable que establece la posición horizontal
 * @param *y Puntero a la variable que establece la posición vertical
 */

void init_rocco_positions(uint8_t level, uint8_t *x, uint8_t *y)
{
  uint8_t rocco_positions[30] = {30, 40, 24, 27, 13, 7, 25, 30, 26, 40, 28, 42, 25, 31, 3, 8, 13, 40, 26, 47, 4, 49, 38, 1, 22, 11, 23, 24, 23, 13};

    level *= 2;

    *x = rocco_positions[level];
    *y = rocco_positions[level + 1];
}

/**
 * @brief Establece el movimiento del personaje principal. Si interactúa con algo y sus consecuencias
 * @param *map Puntero al mapa
 * @param *object_rocco Puntero al sprite del personaje
 */

void rocco_move(Rock_Scroll_Map *map, Rock_Sprite *object_rocco)
{
  int16_t overlaped_object;
  bool can_move = false;
  Rock_Sprite *spr_overlap;

    if (!object_rocco->is_moving ||object_rocco->type_object != ROCCO || !object_rocco->active)
        return;

    if ((can_move = rocco_can_move(object_rocco, map))) {

        if ((overlaped_object = sprites_move(object_rocco, map)) > EMPTY) {

            spr_overlap = sprite_get_object(overlaped_object);
            switch (spr_overlap->type_object) {
                case BOMB:
                case ROCK:
                    rocco_push_rock(object_rocco, spr_overlap, map);
                    break;

                case COIN:
                case DIAMOND:
                    rocco_takes(object_rocco, spr_overlap, map);
                    can_move = true;
                    break;
            }
        }

    }

    if (can_move) {
        if (map->rockmap[object_rocco->x_map][object_rocco->y_map] == GRASS) {
            map->rockmap[object_rocco->x_map][object_rocco->y_map] = GROUND;
            sfx_play(SFX_GRASS);
        }

        if (!rocco_edge(map, object_rocco))
            map_move(map, rocco_get_orientation(object_rocco));
    }
}

/**
 * @brief Establece la estructura de fotogramas para la animación del personaje dependiendo de su orientación.
 * @param *object_rocco Puntero al sprite del personaje
 * @param orientation Orientación del personaje
 */

void rocco_set_action(Rock_Sprite *object_rocco, uint16_t orientation)
{

    if (object_rocco->type_object != ROCCO) // Podría estar muerto
        return;

    if (object_rocco->is_push && orientation == RIGHT)
        orientation = PUSH_RIGHT;

    if (object_rocco->is_push && orientation == LEFT)
        orientation = PUSH_LEFT;

    switch (orientation) {
        case WAIT_LEFT:
        case WAIT_RIGHT:
            object_rocco->h_move = 0;
            object_rocco->v_move = 0;
            object_rocco->num_frames = 1;
            object_rocco->current_frame = 0;
            object_rocco->is_moving = false;
            object_rocco->frames = &rocco[WAIT_LEFT];
            object_rocco->is_push = false;
            break;

        case RIGHT:

            if (object_rocco->h_move != 1) {
                object_rocco->h_move = 1;
                object_rocco->v_move = 0;
                object_rocco->num_frames = 8;
                object_rocco->current_frame = 0;
                object_rocco->is_moving = true;
                object_rocco->frames = &rocco[RIGHT];
            }
            break;

        case LEFT:

            if (object_rocco->h_move != -1) {
                object_rocco->h_move = -1;
                object_rocco->v_move = 0;
                object_rocco->num_frames = 8;
                object_rocco->current_frame = 0;
                object_rocco->is_moving = true;
                object_rocco->frames = &rocco[LEFT];
            }
            break;

        case UP:

            if (object_rocco->v_move != -1) {
                object_rocco->v_move = -1;
                object_rocco->h_move = 0;
                object_rocco->num_frames = 4;
                object_rocco->current_frame = 0;
                object_rocco->is_moving = true;
                object_rocco->frames = &rocco[UP];
            }
            break;

        case DOWN:

            if (object_rocco->v_move != 1) {
                object_rocco->v_move = 1;
                object_rocco->h_move = 0;
                object_rocco->num_frames = 4;
                object_rocco->current_frame = 0;
                object_rocco->is_moving = true;
                object_rocco->frames = &rocco[DOWN];
            }
            break;

        case PUSH_RIGHT:

            if (object_rocco->h_move != 1) {
                object_rocco->h_move = 1;
                object_rocco->v_move = 0;
                object_rocco->num_frames = 4;
                object_rocco->current_frame = 0;
                object_rocco->is_moving = true;
                object_rocco->frames = &rocco[PUSH_RIGHT];
            }
            break;

        case PUSH_LEFT:

            if (object_rocco->h_move != -1) {
                object_rocco->v_move = 0;
                object_rocco->h_move = -1;
                object_rocco->num_frames = 4;
                object_rocco->current_frame = 0;
                object_rocco->is_moving = true;
                object_rocco->frames = &rocco[PUSH_LEFT];
            }
            break;

    }

}

/**
 * @brief Realiza una serie de comprobaciones a través de las funciones de colisión para determinar si se puede mover por el mapa y la interacción con los diferentes objetos
 * @param *object_rocco Puntero al sprite del  personaje principal
 * @param *map Puntero al mapa
 * @return Devuelve true si se puede mover o false de lo contrario
 */

bool rocco_can_move(Rock_Sprite *object_rocco, Rock_Scroll_Map *map)
{
  bool walking = true;
  int16_t pos_x, pos_y;
  Collision_Object collision;

    pos_x = object_rocco->x_map + object_rocco->h_move;
    pos_y = object_rocco->y_map + object_rocco->v_move;

    walking = sprites_can_move(pos_x, pos_y, object_rocco, map, GRASS);

    if (!walking)
        return walking;

    collision.object_type   = OBJECT_SPRITE;
    collision.ignore_object = 0;
    collision_sprite(object_rocco, map, &collision);
    walking = !collision.collision;

    if (collision.object) {

        switch (collision.object->type_object) {
            case BOMB:
            case ROCK:
                rocco_push_rock(object_rocco, collision.object, map);
                break;

            case COIN:
            case DIAMOND:
                rocco_takes(object_rocco, collision.object, map);
                walking = true;
                break;

            default:
                sprites_death(map, object_rocco, true);
                break;

        }
    }

    return walking;
}

/**
 * @brief Devuelve la orientación del personaje
 * @param *object_rocco Puntero al sprite del personaje principal
 * @return Devuelve la orientación
 */

uint8_t rocco_get_orientation(Rock_Sprite *object_rocco)
{
  uint8_t orientation;

    if (object_rocco->h_move == 1)
        orientation = RIGHT;
    else if (object_rocco->h_move == -1)
        orientation = LEFT;
    else if (object_rocco->v_move == -1)
        orientation = UP;
    else if (object_rocco->v_move == 1)
        orientation = DOWN;

    return orientation;
}

/**
 * @brief Comprueba si el personaje ronda los límites del mapa para poder realizar o no el desplazamiento del mapa
 * @param *map Puntero al mapa
 * @param *object_rocco Puntero al sprite del personaje principal
 * @return Devuelve verdadero si ronda los límites, de lo contrario devuelve falso
 */

bool rocco_edge(Rock_Scroll_Map *map, Rock_Sprite *object_rocco)
{
  bool edge = false;

    if (object_rocco->h_move)
        edge = (
                ((MAXMAP_UNITS_X - object_rocco->x_map) < SCROLL_W || object_rocco->x_map <= SCROLL_W) &&
                (map->screen_x >= EDGE_WIDTH || map->screen_x <= 0)
               ) ? true : false;
    else if (object_rocco->v_move)
        edge = (
                ((MAXMAP_UNITS_Y - object_rocco->y_map) < SCROLL_H || object_rocco->y_map <= SCROLL_H) &&
                (map->screen_y >= EDGE_HEIGHT || map->screen_y <= 0)
               ) ? true : false;

    return edge;
}

/**
 * @brief Cambia los fotogramas del personaje principal cuando empuja una roca o una mina. Altera dichos tipos de sprites para que se muevan junto al personaje
 * @param *object_rocco Puntero al sprite del personaje principal
 * @param *object_push Puntero al sprite que será empujado
 * @param *map Puntero al mapa
 */

void rocco_push_rock(Rock_Sprite *object_rocco, Rock_Sprite *object_push, Rock_Scroll_Map *map)
{
    map_clean_reserved(object_push->x_map, object_push->y_map, map);

    if (object_rocco->v_move)
        return;

    if (!object_rocco->is_push) {
        object_rocco->is_push = true;
        object_rocco->h_move = 0;
    }

    if (!object_push->v_move) {
        object_push->is_push = true;
        object_push->h_move  = object_rocco->h_move;
        object_push->move    = sprites_pushed_move;
    }

}

/**
 * @brief Recoge diamantes, monedas o comprueba que puede pasar de nivel a través de la puerta
 * @param *object_rocco Puntero al sprite del personaje principal
 * @param *objec Objeto con el que interactúa
 * @param *map Puntero al mapa
 */

void rocco_takes(Rock_Sprite *object_rocco, Rock_Sprite *object, Rock_Scroll_Map *map)
{
  double screen_x, screen_y;
  uint8_t new_x, new_y;

    screen_x = object_rocco->x_map * TILE_SIZE + object_rocco->offset_x + (SHIFT_TILE * object_rocco->h_move);
    screen_y = object_rocco->y_map * TILE_SIZE + object_rocco->offset_y + (SHIFT_TILE * object_rocco->v_move);

    new_x = (uint8_t)lrint(screen_x / TILE_SIZE);
    new_y = (uint8_t)lrint(screen_y / TILE_SIZE);

    if (new_x == object->x_map && new_y == object->y_map) {

        if (object->type_object == DIAMOND) {
            sfx_play(SFX_DIAMOND);
            map->diamonds --;
            map->points += 10;

            if (map->diamonds <= 0) {
                sfx_play(SFX_DOOR);
                map_set_door(map, OPEN_DOOR);
                map->diamonds = 0;
            }

        } else {
            sfx_play(SFX_COIN);
            map->points += map->level + 1;
        }

        map->update_score = true;
        map->objects_map[new_x][new_y] = EMPTY;
        map_clean_reserved(new_x, new_y, map);

    }

}

/**
 * @brief Si el personaje entra dentro del perímetro de una explosión se marcará la animación en la que se deshace en ceniza
 * @param *map Puntero al mapa
 * @param *object_rocco Puntero al sprite del personaje principal
 */

void rocco_burned(Rock_Scroll_Map *map, Rock_Sprite *object_rocco)
{
    if (sprites_destroy(map, object_rocco)) {
        map->deaths[object_rocco->x_map][object_rocco->y_map] = EMPTY;
        map->objects_map[object_rocco->x_map][object_rocco->y_map] = EMPTY;
        map->rocco_death = true;
    }

}