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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "sprites.h"
#include "rocco.h"
#include "screen.h"

/**
 * @brief Carga el mapa del disco determinado el nivel
 * @param *map Puntero a estructura de tipo mapa donde se almacenarán los datos cargados
 */

void map_load(Rock_Scroll_Map *map)
{
  FILE *fp;
  uint8_t x, y;

    if ((fp = fopen("rockpush.map", "rb"))) {
        /* Originalmente había un margen lateral y superior como buffer. Parcheamos con un borde */
        memset(map->rockmap, GRASS, (uint8_t)MAXMAP_MASK);
        memset(map->deaths, -1, (uint8_t)MAXMAP_MASK);
        memset(map->objects_map, -1, (uint8_t)MAXMAP_MASK);

        fseek(fp, map->level * (MAXMAP + 4), 0);
        map->diamonds = getw(fp);

//        fseek(fp, 2, SEEK_CUR);

        for(x = 0; x < MAXMAP_UNITS_X; x ++){
            for(y = 0; y < MAXMAP_UNITS_Y; y ++){
                map->rockmap[x][y]= getc(fp);
            }
        }
        //fread(map->rockmap, 2500, 1, fp);
        fclose(fp);

    } else {
        puts("No se ha podido abrir el mapa");
        exit(EXIT_FAILURE);
    }

    map_get_objets_entities(map);
}

/**
 * @brief Lee el mapa y dibujará los diferentes elementos. Necesita dos pasadas, una para los objetos fijos y otra para los sprites
 * @param *screen_data Puntero a la estructura donde se almacena la información relacionada con SDL, mapas de bits y texturas
 * @param *map Puntero al mapa
 * @param loop Controla la recursividad para las dos pasadas
 */

void map_show(Rock_Screen *screen_data, Rock_Scroll_Map *map, bool loop)
{
    if (!screen_data->blit_surface && SDL_SetRenderTarget(screen_data->screen, screen_data->dump_texture) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se ha podido asignar el render: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    map->y = (map->screen_y >> SHIFT_TILE);

    for (map->tmp_y = 0; map->tmp_y <= (map->view_height + TILE_SIZE); map->tmp_y += TILE_SIZE) {
        map->x = (map->screen_x >> SHIFT_TILE);

        for (map->tmp_x = 0; map->tmp_x <= (map->view_width + TILE_SIZE); map->tmp_x += TILE_SIZE) {
            (loop) ? sprites_set_sprite_from_map(screen_data, map) : sprites_set_tile_from_map(screen_data, map);
            map->x ++;
        }

        map->y ++;
    }

    /* Pintamos el mapa en dos pasadas para superponer los objetos móviles sobre los estáticos */
    if (!loop)
        map_show(screen_data, map, true);

}

/**
 * @brief Lee el mapa y determina la cantidad de objetos sprite hay en él para poder reservar la memoria
 * @param *map Puntero al mapa
 */

void map_get_objets_entities(Rock_Scroll_Map *map)
{
  uint8_t x, y;
  int16_t objects = 0;

    map->diamonds = 0;
    init_rocco_positions(map->level, &x, &y);
    map->rockmap[x][y] = ROCCO;
    map_set_screen_position(map, x, y);

    for (y = 0; y < MAXMAP_UNITS_Y; y ++) {
        for (x = 0; x < MAXMAP_UNITS_X; x ++) {
            map->rockmap[x][y] = patch_older_map(map->rockmap[x][y]);
            map->deaths[x][y]  = -1;

            switch (map->rockmap[x][y]) {
                case DIAMOND:
                    map->diamonds ++;
                case COIN:
                case BOMB:
                case ROCK:
                case DEMONIETE:
                case DUMMY:
                case FLY:
                case GUZZLER:
                case ROCCO:
                    objects ++;
                    break;
            }

        }
    }

    map->objects = objects;
    map_set_door(map, DOOR);
}

/**
 * @brief Parchea algunos valores del mapa heredado de la primera versión de RockPush (1995) para adecuarla a los nuevos personajes.
 * @param map_value Valor del mapa original
 * @return Devuelve el valor mapeado
 */

uint8_t patch_older_map(uint8_t map_value)
{
    switch (map_value) {
        case 0:
            if ((rand() % 50) == 1)
                map_value = (uint8_t)GROUND_BONE;

            break;

        case 13:
            map_value = FLY;
            break;

        case 20:
        case 21:
        case 22:
            map_value = GUZZLER;
            break;

        case 25:
            map_value = DEMONIETE;
            break;

        case 75:
            map_value = OPEN_DOOR;
            break;

        case 60:
            map_value = DOOR;
            break;
    }

    return map_value;
}

/**
 * @brief Establece la posición X e Y para cada nivel. Depende de donde esté colocado el personaje principal
 * @param *map Puntero al mapa
 * @param x Posición X a detereminar
 * @param y Posición Y a detereminar
 */

void map_set_screen_position(Rock_Scroll_Map *map, int8_t x, int8_t y)
{
  int8_t tx, ty, edge_x, edge_y;

    tx     = (MAP_WIDTH / 2) + 1;
    ty     = (MAP_HEIGHT / 2) + 1;
    edge_x = MAXMAP_UNITS_X - MAP_WIDTH - 1;
    edge_y = MAXMAP_UNITS_Y - MAP_HEIGHT - 1;

    x -= tx;
    y -= ty;

    x = (x >= edge_x) ? edge_x : x;
    y = (y >= edge_y) ? edge_y : y;

    x = (x >= 0) ? x : 0;
    y = (y >= 0) ? y : 0;

    map->screen_x = x * TILE_SIZE;
    map->screen_y = y * TILE_SIZE;
}

/**
 * @brief Mueve el mapa según la posición del personaje principal. Establece scroll
 * @param *map Puntero al mapa
 * @param orientation Sentido del movimiento, hacia donde se tiene que desplazar
 */

void map_move(Rock_Scroll_Map *map, uint8_t orientation)
{

    switch(orientation) {

        case UP:
            map->screen_y = ((map->screen_y - map->scroll_shift) > 0) ? (map->screen_y -= map->scroll_shift) : 0;
            break;

        case DOWN:
            map->screen_y = ((map->screen_y + map->scroll_shift) < EDGE_HEIGHT) ? (map->screen_y += map->scroll_shift) : EDGE_HEIGHT;
            break;

        case LEFT:
            map->screen_x = ((map->screen_x - map->scroll_shift) > 0) ? (map->screen_x -= map->scroll_shift) : 0;
            break;

        case RIGHT:
            map->screen_x = ((map->screen_x + map->scroll_shift) < EDGE_WIDTH) ? (map->screen_x += map->scroll_shift) : EDGE_WIDTH;
            break;

    }
}

/**
 * @brief Establece las posiciones en el mapa de la puerta para poder pasar de nivel.
 * @param *map Puntero al mapa
 * @param status Establece si la puerta está abierta o cerrada. Si está abierta indica que es posible pasar de nivel
 */

void map_set_door(Rock_Scroll_Map *map, uint8_t status)
{
  uint8_t level, door[] = {38, 13, 24, 4, 13, 8, 2, 39, 7, 15, 22, 22, 49, 43, 10, 44, 1, 1, 28, 28, 40, 1, 39, 1, 32, 4, 23, 1, 1, 49};

    level = map->level * 2;
    map->rockmap[door[level]][door[level + 1]] = status;
}

/**
 * @brief Elimina de los mapas las marcas de sprites especiales
 * @param x_map La posición X del objeto a partir de la cual se trazará el perímetro horizontal
 * @param y_map La posición Y del objeto a partir de la cual se trazará el perímetro vertical
 * @param *map Puntero al mapa
 */

void map_clean_reserved(int8_t x_map, int8_t y_map, Rock_Scroll_Map *map)
{
    int16_t from_x, from_y, to_x, to_y, x, y;

    from_x = (x_map - 1) > 0 ? x_map - 1 : 0;
    to_x   = x_map + 1;
    from_y = (y_map - 1 > 0) ? y_map - 1 : 0;
    to_y   = y_map + 1;

    for (y = from_y; y <= to_y && y < MAXMAP_UNITS_Y; y ++) {
        for (x = from_x; x <= to_x && x < MAXMAP_UNITS_X; x++) {

            if (map->objects_map[x][y] == RESERVED)
                map->objects_map[x][y] = EMPTY;

        }
    }

}
