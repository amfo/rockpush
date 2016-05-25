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
#include <stdio.h>
#include "screen.h"
#include "rockpush.h"
#include "sprites.h"
#include "demoniete.h"
#include "dummy.h"
#include "fly.h"
#include "guzzler.h"
#include "rocco.h"
#include "collision.h"
#include "explosion.h"
#include "sfx.h"

/**
 * @brief Carga las texturas en dos formatos
 * @param *screen_data Puntero a la estructura de tipo Rock_Screen con la información de texturas y superficies a usar
 */

void sprites_load_textures(Rock_Screen *screen_data)
{
    if ((screen_data->sprites_surface = IMG_Load(SURFACE)) == NULL) {
        printf("Archivo de texturas no encontrado %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if ((screen_data->sprites_texture = IMG_LoadTexture(screen_data->screen, SURFACE)) == NULL) {
        printf("Archivo de texturas no encontrado %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetTextureBlendMode(screen_data->sprites_texture, SDL_BLENDMODE_BLEND);
}

/**
 * @brief Libera las texturas cargadas
 * @param *screen_data Puntero a la estructura de tipo Rock_Screen
 */

void sprites_free_textures(Rock_Screen *screen_data)
{
    SDL_FreeSurface(screen_data->sprites_surface);
    SDL_DestroyTexture(screen_data->sprites_texture);
}

/**
 * @brief Establece la secuencia, en forma de lista enlazada, de los fotogramas para la animación de los diferentes sprites
 * @param *objec Puntero a la estructura anidada de tipo FRAME
 * @param frames Número de fotogramas que tendrá la serie
 * @param shift_frame_x Desplazamiento con respecto a la posición 0,0 del mapa de bits donde se encuentran las texturas. Es múltiplo de frame_w
 * @param shift_frame_y Desplazamiento con respecto a la posición 0,0
 * @param frame_w Ancho del fotograma. Es una fracción del ancho del fotograma establecido en el preprocesador frame_w * TILE_SIZE
 * @param frame_h Alto del fotogram
 */

void sprite_set_frames_to_sprite(FRAME *object, int16_t frames, int16_t shift_frame_x, int16_t shift_frame_y, int16_t frame_w, int16_t frame_h)
{
  int16_t frame;
  FRAME *fr;
  FRAME *curr_fr;

    fr = (FRAME *) malloc(frames * sizeof(FRAME));
    if (fr == NULL) {
        puts("No se han podido reservar los fotogramas en memoria");
        exit(EXIT_FAILURE);
    }

    object->size.h = TILE_SIZE * frame_h;
    object->size.w = TILE_SIZE * frame_w;
    object->size.x = TILE_SIZE * shift_frame_x;
    object->size.y = TILE_SIZE * shift_frame_y;
    object->next_frame = fr;

    curr_fr = fr;

    for (frame = 1; frame < frames; frame ++) {
        curr_fr->size.h = TILE_SIZE * frame_h;
        curr_fr->size.w = TILE_SIZE * frame_w;
        curr_fr->size.x = TILE_SIZE * (frame * frame_w + shift_frame_x);
        curr_fr->size.y = TILE_SIZE * shift_frame_y;

        if (frame + 1 < frames) {
            curr_fr ++;
            curr_fr->next_frame = curr_fr;
        } else
            curr_fr->next_frame = NULL;

    }

}

void sprites_set_tile_ground(void)
{
    tiles[GROUND].next_frame = NULL;
    tiles[GROUND].size.h = tiles[GROUND].size.w = TILE_SIZE;
    tiles[GROUND].size.x = TILE_SIZE * 2;
    tiles[GROUND].size.y = 0;

    tiles[GROUND_BONE].next_frame = NULL;
    tiles[GROUND_BONE].size.h = tiles[GROUND_BONE].size.w = TILE_SIZE;
    tiles[GROUND_BONE].size.x = TILE_SIZE * 3;
    tiles[GROUND_BONE].size.y = 0;
}

void sprites_set_tile_grass(void)
{
    tiles[GRASS].next_frame = NULL;
    tiles[GRASS].size.h = tiles[GRASS].size.w = TILE_SIZE;
    tiles[GRASS].size.x = 0;
    tiles[GRASS].size.y = 0;
}

void sprites_set_tile_brick(void)
{
    tiles[BRICKS].next_frame = NULL;
    tiles[BRICKS].size.h = tiles[BRICKS].size.w = TILE_SIZE;
    tiles[BRICKS].size.x = TILE_SIZE * 10;
    tiles[BRICKS].size.y = 0;
}

void sprites_set_tile_door(void)
{
    tiles[DOOR].next_frame = NULL;
    tiles[DOOR].size.h = tiles[DOOR].size.w = TILE_SIZE;
    tiles[DOOR].size.x = TILE_SIZE * 11;
    tiles[DOOR].size.y = 0;
}

void sprites_set_tile_opendoor(void)
{
    tiles[OPEN_DOOR].next_frame = NULL;
    tiles[OPEN_DOOR].size.h = tiles[OPEN_DOOR].size.w = TILE_SIZE;
    tiles[OPEN_DOOR].size.x = TILE_SIZE * 12;
    tiles[OPEN_DOOR].size.y = 0;
}

void sprites_set_tile_bomb(void)
{
    tiles[BOMB].next_frame = NULL;
    tiles[BOMB].size.h = tiles[BOMB].size.w = TILE_SIZE;
    tiles[BOMB].size.x = TILE_SIZE * 17;
    tiles[BOMB].size.y = 0;
}

void sprites_set_demoniete(void)
{
    sprite_set_frames_to_sprite(&demon[WAIT_LEFT], 4, 0, 4, 1, 1);
    sprite_set_frames_to_sprite(&demon[WAIT_RIGHT], 4, 0, 5, 1, 1);
    sprite_set_frames_to_sprite(&demon[LEFT], 12, 4, 4, 1, 1);
    sprite_set_frames_to_sprite(&demon[RIGHT], 12, 4, 5, 1, 1);
    sprite_set_frames_to_sprite(&demon[FALL_LEFT], 2, 6, 4, 1, 1);
    sprite_set_frames_to_sprite(&demon[FALL_RIGHT], 2, 6, 5, 1, 1);
}

void sprites_set_guzzler(void)
{
    sprite_set_frames_to_sprite(&guzzler[LEFT], 2, 18, 4, 1, 1);
    sprite_set_frames_to_sprite(&guzzler[RIGHT], 2, 18, 5, 1, 1);
    sprite_set_frames_to_sprite(&guzzler[DOWN], 2, 20, 4, 1, 1);
    sprite_set_frames_to_sprite(&guzzler[UP], 2, 20, 5, 1, 1);
}

void sprites_set_fly(void)
{
    sprite_set_frames_to_sprite(&fly[RIGHT], 2, 16, 4, 1, 1);
    sprite_set_frames_to_sprite(&fly[LEFT], 2, 16, 5, 1, 1);
}

void sprites_set_rocco(void)
{
    sprite_set_frames_to_sprite(&rocco[WAIT_LEFT], 1, 16, 6, 1, 1);
    sprite_set_frames_to_sprite(&rocco[WAIT_RIGHT], 1, 16, 6, 1, 1);
    sprite_set_frames_to_sprite(&rocco[LEFT], 8, 0, 6, 1, 1);
    sprite_set_frames_to_sprite(&rocco[RIGHT], 8, 8, 6, 1, 1);
    sprite_set_frames_to_sprite(&rocco[DOWN], 4, 16, 6, 1, 1);
    sprite_set_frames_to_sprite(&rocco[UP], 4, 20, 6, 1, 1);
    sprite_set_frames_to_sprite(&rocco[PUSH_LEFT], 4, 0, 7, 1, 1);
    sprite_set_frames_to_sprite(&rocco[PUSH_RIGHT], 4, 4, 7, 1, 1);
}

void sprites_set_tiles_textures(void)
{
    sprites_set_tile_ground();
    sprites_set_tile_grass();
    sprites_set_tile_brick();
    sprites_set_tile_door();
    sprites_set_tile_opendoor();

    sprites_set_tile_bomb();
    sprite_set_frames_to_sprite(&tiles[ROCK], 6, 4, 0, 1, 1);
    sprite_set_frames_to_sprite(&tiles[DIAMOND], 4, 13, 0, 1, 1);
    sprite_set_frames_to_sprite(&tiles[COIN], 8, 18, 0, 1, 1);
    sprite_set_frames_to_sprite(&tiles[DUMMY], 4, 22, 4, 1, 1);
    sprite_set_frames_to_sprite(&tiles[EXPLOSION], 9, 0, 1, 3, 3);
    sprite_set_frames_to_sprite(&tiles[BLOOD], 16, 8, 7, 1, 1);
    sprite_set_frames_to_sprite(&tiles[ROCCO_BURNED], 5, 22, 5, 1, 1);

    sprites_set_demoniete();
    sprites_set_guzzler();
    sprites_set_fly();
    sprites_set_rocco();
}

/**
 * @brief Representa en el buffer gráfico los diferentes sprites visibles del mapa en su posición relativa
 * @param *screen_data Puntero a la estructura de tipo Rock_Screen
 * @param *map Puntero a la estructura de tipo Rock_Scroll_Map
 */

void sprites_set_sprite_from_map(Rock_Screen *screen_data, Rock_Scroll_Map *map)
{
  SDL_Rect target;
  SDL_Rect object_frame;
  Rock_Sprite *spr;
  int16_t result = 0, object_index, blood_index, screen_x, screen_y;

    object_index = map->objects_map[map->x][map->y];
    blood_index  = map->deaths[map->x][map->y];
    screen_x     = map->screen_x & TILE_MASK;
    screen_y     = map->screen_y & TILE_MASK;
    target.x     = map->tmp_x - screen_x;
    target.y     = map->tmp_y - screen_y;
    target.h     = target.w = TILE_SIZE;

    /* Diferenciamos objetos móviles de estáticos para evitar superposición en caso de movimientos autónomos */
    if (object_index > EMPTY && object_index < map->objects) {
        spr = sprite_get_object(object_index);
        target.x += spr->offset_x; /* Corregimos la posición del objeto móvil en pantalla*/
        target.y += spr->offset_y;

        spr->is_visible = true; // Si es visible podemos iniciar los efectos de sonido

        switch (spr->type_object) {
            case BOMB:
                result = screen_copy_texture(screen_data, &tiles[BOMB].size, &target);
                break;

            case EXPLOSION:
                target.x = (map->tmp_x - TILE_SIZE) - screen_x;
                target.y = (map->tmp_y - TILE_SIZE) - screen_y;
                target.h = target.w = TILE_SIZE * EXPLOSION_SIZE;

            case GUZZLER:
            case FLY:
            case DUMMY:
            case DEMONIETE:
            case DIAMOND:
            case ROCK:
            case COIN:
            case ROCCO:
            case ROCCO_BURNED:
                sprite_get_current_frame(&object_frame, spr); /* Obtenemos la secuencia actual antes de actualizarla */
                result = screen_copy_texture(screen_data, &object_frame, &target);
                break;
        }
    }

    if (blood_index > EMPTY && blood_index < map->objects) {
        target.x = map->tmp_x - screen_x;
        target.y = map->tmp_y - screen_y;

        spr = sprite_get_object(blood_index);
        target.x += spr->offset_x;
        target.y += spr->offset_y;
        sprite_get_current_frame(&object_frame, spr);
        result = screen_copy_texture(screen_data, &object_frame, &target);
    }

    if (result < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se ha podido copiar la textura: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

}

/**
 * @brief Representa en el buffer gráfico los diferentes elementos estáticos visibles del mapa en su posición relativa
 * @param *screen_data Puntero a la estructura de tipo Rock_Screen
 * @param *map Puntero a la estructura de tipo Rock_Scroll_Map
 */

void sprites_set_tile_from_map(Rock_Screen *screen_data, Rock_Scroll_Map *map)
{
  SDL_Rect target;
  int16_t result = 0, value;

    value    = map->rockmap[map->x][map->y];
    target.x = map->tmp_x - (map->screen_x & TILE_MASK);
    target.y = map->tmp_y - (map->screen_y & TILE_MASK);
    target.h = target.w = TILE_SIZE;

    /* Diferenciamos objetos móviles de estáticos para evitar superposición en caso de movimientos autónomos */
    result = screen_copy_texture(screen_data, &tiles[GROUND].size, &target);

    switch (value) {
        case GROUND_BONE:
        case GRASS:
        case BRICKS:
        case DOOR:
        case OPEN_DOOR:
            result = screen_copy_texture(screen_data, &tiles[value].size, &target);
            break;
    }

    if (result < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se ha podido copiar la textura: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Comprueba el estado del sprite y ejecuta la función de movimiento.
 * @param *map Puntero a la estructura de tipo Rock_Scroll_Map
 */

void sprites_update(Rock_Scroll_Map *map)
{
  int16_t object_index, x, y, objects;
  Rock_Sprite *spr;

    for (objects = 0; objects < 2; objects ++) {
        for (y = 0; y < MAXMAP_UNITS_Y; y ++) {
            for (x = 0; x < MAXMAP_UNITS_X; x ++) {

                object_index = (objects) ? map->deaths[x][y] : map->objects_map[x][y];

                if (object_index > EMPTY && object_index < map->objects) {

                    spr = sprite_get_object(object_index);
                    if (spr->move != NULL)
                        spr->move(map, spr);

                }
            }
        }
    }
}

/**
 * @brief Función de inicialización. Establece la memoria necesaria para almacenar todos los sprites definidos en el mapa e inicializa los objetos
 * @param *map Puntero a la estructura del mapa
 */

void sprites_set_objects(Rock_Scroll_Map *map)
{
  int16_t x, y, objects, current_object = 0;
  Rock_Sprite *spr;

    objects = map->objects;
    sprites = (Rock_Sprite *) malloc(objects * sizeof(Rock_Sprite));
    if (sprites == NULL) {
        puts("No se han podido reservar los sprites en memoria");
        exit(EXIT_FAILURE);
    }

    spr = sprites;

    for (y = 0; y < MAXMAP_UNITS_Y; y ++) {
        for (x = 0; x < MAXMAP_UNITS_X; x ++) {

            map->objects_map[x][y] = EMPTY;
            switch (map->rockmap[x][y]) {
                case ROCCO:
                    map->rocco_index = current_object;
                case COIN:
                case DIAMOND:
                case BOMB:
                case ROCK:
                case DEMONIETE:
                case GUZZLER:
                case FLY:
                case DUMMY:
                case ROCCO_BURNED:
                    /* Filtramos solo determinados objetos */

                    if (current_object < objects) {
                        sprites_define_object(spr, x, y, map->rockmap[x][y]);
                        spr->index = current_object;
                        map->rockmap[x][y] = GROUND;
                        map->objects_map[x][y] = current_object; /* Guardamos en cuadrante el índice del objeto creado */
                        current_object ++;
                        spr ++;
                    }

                    break;

            }
        }

    }
}

/**
 * @brief Inicializa, según el tipo, las variables que definen el sprite
 * @param *spr Puntero al sprite
 * @param map_x Posición X en el mapa donde se establece el sprite
 * @param map_y Posición Y en el mapa donde se establece el sprite
 * @param object Tipo de objeto definido en la enumeración de la cabecera
 */

void sprites_define_object(Rock_Sprite *spr, int16_t map_x, int16_t map_y, int16_t object)
{
  int16_t orientation;

    spr->x_map              = map_x;
    spr->y_map              = map_y;
    spr->offset_x           = 0;
    spr->offset_y           = 0;
    spr->is_moving          = false;
    spr->is_visible         = false;
    spr->active             = true;
    spr->is_push            = false;
    spr->is_slide           = false;
    spr->h_move             = 0;
    spr->v_move             = 0;
    spr->current_step       = 0;
    spr->steps_next_frame   = 0;
    spr->pause_loop_frame   = 0;
    spr->current_pause      = 0;
    spr->num_frames         = 0;
    spr->min_frames_move    = 0;
    spr->type_object        = object;
    spr->index              = -1;
    spr->move               = NULL;
    spr->change_direction   = NULL;
    spr->wait               = 0;
    spr->collision_radius   = R_COLLISION;

    switch (object) {
        case COIN:
            spr->num_frames            = 8;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 1;
            spr->is_moving             = true;
            spr->collision_radius      = 16;
            spr->frames                = &tiles[COIN];
            spr->move                  = sprites_falls;
            break;

        case DIAMOND:
            spr->num_frames            = 4;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 1;
            spr->pause_loop_frame      = 100;
            spr->is_moving             = true;
            spr->frames                = &tiles[DIAMOND];
            spr->move                  = sprites_falls;
            break;

        case ROCK:
            spr->num_frames            = 6;
            spr->current_frame         = 0;
            spr->frames                = &tiles[ROCK];
            spr->move                  = sprites_falls;
            break;

        case BOMB:
            spr->move                  = sprites_falls;
            break;

        case DEMONIETE:
            orientation = rand() % 2;

            spr->num_frames            = 4;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 2;
            spr->is_moving             = true;
            spr->min_frames_move       = (rand() % 10) + 10;
            spr->frames                = &demon[orientation];
            spr->move                  = demoniete_move;
            spr->change_direction      = demoniete_change_direction;
            break;

        case DUMMY:
            spr->num_frames            = 4;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 4;
            spr->is_moving             = true;
            spr->frames                = &tiles[DUMMY];
            spr->move                  = dummy_move;
            spr->change_direction      = dummy_change_direction;
            break;

        case GUZZLER:
            orientation = (rand() % 4) + 2;

            spr->num_frames            = 2;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 4;
            spr->is_moving             = true;
            spr->frames                = &guzzler[orientation];
            spr->move                  = guzzler_move;
            spr->change_direction      = guzzler_change_direction;
            break;

        case FLY:
            orientation = rand() % 2;
            orientation = (orientation) ? RIGHT : LEFT;

            spr->num_frames            = 2;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 4;
            spr->is_moving             = true;
            spr->frames                = &fly[orientation];
            spr->move                  = fly_move;
            spr->change_direction      = fly_change_direction;
            break;

        case ROCCO:
            orientation = WAIT_LEFT;

            spr->num_frames            = 1;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 1;
            spr->is_moving             = false;
            spr->frames                = &rocco[orientation];
            spr->move                  = rocco_move;
            break;

        case ROCCO_BURNED:
            spr->num_frames            = 5;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 3;
            spr->is_moving             = true;
            spr->frames                = &tiles[ROCCO_BURNED];
            spr->move                  = rocco_burned;
            break;

        case EXPLOSION:
            spr->num_frames            = 9;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 1;
            spr->is_moving             = true;
            spr->frames                = &tiles[EXPLOSION];
            spr->move                  = explosion_end;
            break;

        case ROCCO_DEATH:
        case BLOOD:
            spr->num_frames            = 16;
            spr->current_frame         = 0;
            spr->steps_next_frame      = 2;
            spr->is_moving             = true;
            spr->frames                = &tiles[BLOOD];
            spr->move                  = sprites_death_end;
            break;
    }
}

/**
 * @brief Según el índice de objetos, obtiene el puntero al sprite correspondiente
 * @param object_index Índice del objeto en orden secuencial, por aparición, del mapa
 * @return Devuelve el puntero a la posición del dicho sprite
 */

Rock_Sprite *sprite_get_object(int16_t object_index)
{
    Rock_Sprite *spr = NULL;

    if (object_index > EMPTY) {
        spr  = sprites;
        spr += object_index;
    }

    return spr;
}

/**
 * @brief Obtiene el fotograma actual correspondiente
 * @param *object_frame Puntero a una estructura de tipo SDL_Rect que contiene la información de posiciones, anchura y altura del fotograma
 * @param *spr Puntero al sprite
 */

void sprite_get_current_frame(SDL_Rect *object_frame, Rock_Sprite *spr)
{
  FRAME *current_frame;

    if (spr->frames == NULL)
        return;

    *object_frame = spr->frames->size;

    /* current_pause controla el espacio entre ciclos de animación (un ciclo comprende todo el rango de fotogramas), proporciona un efecto de pausa entre animación y animación */
    spr->current_pause = (!spr->current_frame && !spr->current_step && spr->current_pause < spr->pause_loop_frame) ? spr->current_pause + 1 : 0;
    /* current_step controla el tiempo entre fotograma de la animación */
    spr->current_step  = (!spr->current_pause && spr->current_step  < spr->steps_next_frame) ? spr->current_step + 1 : 0;

    if (spr->active && spr->current_frame < spr->num_frames && spr->is_moving && !spr->current_pause) {

        if (spr->current_frame > 0) {
            current_frame  = spr->frames->next_frame;
            current_frame += spr->current_frame - 1;
            *object_frame  = current_frame->size;
        }

        if (!spr->current_step)
            spr->current_frame ++;

    }

    if (spr->current_frame >= spr->num_frames && !spr->current_step && !spr->current_pause)
        spr->current_frame = 0;

}

/**
 * @brief Mueve el sprite por el mapa y evita que se solape con otro. Calcula la posición relativa en base a la coordenada real
 * @param *spr Puntero al sprite
 * @param *map Puntero a la estructura del mapa
 * @return Devuelve el índice en el caso de que haya superposición o el valor de coordenada vacía
 */

int16_t sprites_move(Rock_Sprite *spr, Rock_Scroll_Map *map)
{
  double screen_x, screen_y;
  int16_t new_x, new_y, move_x, move_y, offset_x, offset_y, object_index;

    offset_x = spr->offset_x + spr->h_move * SHIFT_TILE;
    offset_y = spr->offset_y + spr->v_move * SHIFT_TILE;

    screen_x = spr->x_map * TILE_SIZE + offset_x;
    screen_y = spr->y_map * TILE_SIZE + offset_y;
    move_x   = spr->x_map + spr->h_move;
    move_y   = spr->y_map + spr->v_move;
    new_x = (int16_t)lrint(screen_x / TILE_SIZE);
    new_y = (int16_t)lrint(screen_y / TILE_SIZE);
    object_index = map->objects_map[new_x][new_y];

    if (object_index != EMPTY && object_index != spr->index)
        return object_index;

    spr->offset_x = offset_x;
    spr->offset_y = offset_y;

    if (new_x == move_x) {
        if (spr->h_move == 1)
            spr->offset_x = -TILE_SIZE + spr->offset_x;
        else if (spr->h_move == -1)
            spr->offset_x = TILE_SIZE + spr->offset_x;
    }

    if (new_y == move_y) {
        if (spr->v_move == 1)
            spr->offset_y = -TILE_SIZE + spr->offset_y;
        else if (spr->v_move == -1)
            spr->offset_y = TILE_SIZE + spr->offset_y;
    }

    if (map->objects_map[spr->x_map][spr->y_map] == spr->index) {

        map->objects_map[spr->x_map][spr->y_map] = EMPTY;

        spr->x_map = new_x;
        spr->y_map = new_y;

        map->objects_map[spr->x_map][spr->y_map] = spr->index;

        if (spr->min_frames_move > 0)
            spr->min_frames_move --;
    }

    return EMPTY;
}

/**
 * @brief Desplaza el sprite. La diferencia con la anterior es que no mueve el obejto en el mapa hasta haber completado la transición completa de pantalla
 * @param *spr Puntero al sprite
 * @param *map Puntero a la estructura del mapa
 */

void sprites_shift(Rock_Sprite *spr, Rock_Scroll_Map *map)
{
  uint8_t shift, move_x = 0, move_y = 0;
  int16_t offset_y = 0;

    if (spr->h_move != 0) {

        shift = spr->x_map;
        move_x = spr->x_map + spr->h_move;

        if ((spr->offset_x > TILE_SIZE && move_x < MAXMAP_UNITS_X) || (spr->offset_x < -TILE_SIZE && move_x > 0)) {
            spr->offset_x = 0;
            shift = move_x;
        }

        if (spr->offset_x == 0 && spr->x_map != shift) {
            map->objects_map[spr->x_map][spr->y_map] = EMPTY;
            spr->x_map      = shift;
            spr->is_slide   = false;
            map->objects_map[spr->x_map][spr->y_map] = spr->index;

            if (spr->min_frames_move > 0)
                spr->min_frames_move --;

        }

        spr->offset_x += spr->h_move * SHIFT_TILE;

    }

    if (spr->v_move != 0) {
        offset_y = spr->offset_y + spr->v_move * SHIFT_TILE;
        move_y   = spr->y_map + spr->v_move;
        shift    = spr->y_map;

        if ((offset_y > TILE_SIZE && move_y < MAXMAP_UNITS_Y) || (offset_y < -TILE_SIZE && move_y > 0)) {
            offset_y = 0;
            shift = move_y;
        }

        if (!offset_y) {
            map->objects_map[spr->x_map][spr->y_map] = EMPTY;
            spr->y_map      = shift;
            spr->is_slide   = false;
            map->objects_map[spr->x_map][spr->y_map] = spr->index;

            if (spr->min_frames_move > 0)
                spr->min_frames_move --;

        }

        spr->offset_y = offset_y;
    }
}

/**
 * @brief Para rocas, bombas y demonietes. Simula la caída si no hay nada debajo y comprueba con qué colisiona y sus consecuencias
 * @param *map Puntero a la estructura del mapa
 * @param *spr Puntero al sprite
 */

void sprites_falls(Rock_Scroll_Map *map, Rock_Sprite *spr)
{
  Rock_Sprite *object;
  uint8_t has_collision;
  int16_t move_y, index;
  Collision_Object collision;

    move_y = spr->y_map + 1;
    index  = map->objects_map[spr->x_map][move_y];

    if (!spr->active)
        return;

    if (spr->is_slide) {
        sprites_shift(spr, map);
        return;
    }

    has_collision = collision_map(map, spr->x_map, spr->y_map, DOWN);

    if (has_collision && !spr->v_move) {
        spr->move = sprites_slide;
        return;
    }

    collision.ignore_object = 0;
    collision.object_type   = OBJECT_SPRITE;
    collision_sprite(spr, map, &collision);
    /* Si no hay nada debajo activamos el movimiento vertical */

    if ((has_collision == OBJECT_SPRITE || collision.collision) && spr->v_move == 1) {
        spr->is_slide = false;
        object = NULL;

        if (collision.collision && collision.object != NULL)
            object = collision.object;
        else if (move_y < MAXMAP_UNITS_Y && index > EMPTY)
            object = sprite_get_object(index);

        if (object != NULL && object->active) {
            spr->h_move = 0;

            if (spr->type_object == BOMB)
                explosion_set(map, spr);

            switch (object->type_object) {
                case DIAMOND:
                case COIN:
                case ROCK:
                    spr->v_move = 0;
                    break;

                case BOMB:
                    spr->v_move = 0;
                    explosion_set(map, object);
                    break;

                case EXPLOSION:
                    break;

                case FLY:
                    fly_to_diamonds(map, object);
                    break;

                default:
                    sprites_death(map, object, true);
                    break;
            }

        }

        collision.object_type = OBJECT_STATIC;
        collision_sprite(spr, map, &collision);
        if (collision.collision)
            spr->v_move = 0;
    }

    if (has_collision && spr->v_move && spr->type_object == BOMB && index != RESERVED)
        explosion_set(map, spr);

    if (has_collision && spr->v_move && spr->type_object == DEMONIETE && index != RESERVED)
        spr->move = demoniete_move;

    if (has_collision) {
        if (spr->is_visible)
            sfx_play(SFX_FALL);

        spr->is_visible = false;
        spr->v_move     = 0;
        spr->is_slide   = false;
    } else {
        map->objects_map[spr->x_map][move_y] = RESERVED;
        spr->v_move = 1;
    }

    if (spr->v_move) {
        spr->h_move   = 0;
        spr->is_slide = true;
    }

    if (!spr->h_move && spr->type_object == ROCK)
        spr->is_moving = false;

}

/**
 * @brief Desplaza determinados tipos de sprites, como rocas o bombas, si el terreno no es firme
 * @param *map Puntero al mapa
 * @param *spr Puntero al sprite
 */

void sprites_slide(Rock_Scroll_Map *map, Rock_Sprite *spr)
{
  int8_t h_move = 0;
  int16_t move_y, move_x;
  bool slide = false;
  Rock_Sprite *object;

    move_y = spr->y_map + 1;

    if (map->rockmap[spr->x_map][move_y] == GRASS) {
        spr->move     = sprites_falls;
        spr->h_move   = 0;
        spr->is_slide = false;
        return;
    }

    if (map->rockmap[spr->x_map][move_y] == BRICKS)
        slide = true;

    if (!slide && !spr->is_slide && map->objects_map[spr->x_map][move_y] > EMPTY) {

        object = sprite_get_object(map->objects_map[spr->x_map][move_y]);

        switch (object->type_object) {

            case ROCK:
            case DIAMOND:
            case BOMB:
            case COIN:
                slide = true;
                break;

        }

        if (object->h_move || object->v_move || object->is_slide)
            slide = false;
    }

    if (slide && !spr->is_slide) {

        move_x = spr->x_map + 1;

        if (
            !spr->v_move &&
            map->objects_map[move_x][spr->y_map] == EMPTY &&
            (map->rockmap[move_x][spr->y_map] == GROUND || map->rockmap[move_x][spr->y_map] == GROUND_BONE) &&
            map->objects_map[move_x][move_y] == EMPTY &&
            (map->rockmap[move_x][move_y] == GROUND || map->rockmap[move_x][move_y] == GROUND_BONE)
        )
            h_move = 1;

        move_x = spr->x_map - 1;

        if (
            !spr->v_move &&
            map->objects_map[move_x][spr->y_map] == EMPTY &&
            (map->rockmap[move_x][spr->y_map] == GROUND || map->rockmap[move_x][spr->y_map] == GROUND_BONE) &&
            map->objects_map[move_x][move_y] == EMPTY &&
            (map->rockmap[move_x][move_y] == GROUND || map->rockmap[move_x][move_y] == GROUND_BONE)
        )
            h_move = (h_move) ? 2 : -1;


        if (h_move == 2) {
            h_move = rand() % 2;
            h_move = (!h_move) ? -1 : 1;
        }

    }

    move_x = spr->x_map + h_move;

    // Evitamos el desplazamiento si alrededor hay piezas o reservas
    if (
        slide &&
        (map->objects_map[spr->x_map][spr->y_map -1] > EMPTY || map->objects_map[spr->x_map][spr->y_map -1] == RESERVED) &&
        (map->objects_map[move_x][spr->y_map -1] > EMPTY || map->objects_map[move_x][spr->y_map -1] == RESERVED)
    )
        h_move = 0;

    if (h_move) {//) && sprites_can_move(move_x, spr->y_map, spr, map, RESERVED)) {
        spr->is_moving  = (spr->type_object == ROCK) ? true : spr->is_moving;
        spr->h_move     = h_move;
        spr->is_slide   = true;
        map->objects_map[move_x][spr->y_map] = RESERVED;
    }

    if (!spr->is_slide) {
        spr->h_move     = 0;
        spr->move       = sprites_falls;
        spr->is_moving  = (spr->type_object == ROCK) ? false : spr->is_moving;
    } else
        sprites_shift(spr, map);

}

/**
 * @brief Comprueba, mediante diferentes métodos, si el sprite se puede mover y sus consecuencias
 * @param pos_x Coordenada X a averiguar si es posible el movimiento
 * @param pos_y Coordenada Y a averiguar si es posible el movimiento
 * @param *spr Puntero al sprite
 * @param *map Puntero al mapa
 * @param ignore_object Valor correspondiente a la enumeración de tipo de objetos que se obviará en la comprobación
 * @return Devuelve verdadero si puede moverse, de lo contrario devuelve falso
 */

bool sprites_can_move(int16_t pos_x, int16_t pos_y, Rock_Sprite *spr, Rock_Scroll_Map *map, int8_t ignore_object)
{
  bool walking = true;
  Collision_Object collision;
  int16_t offset;

    offset = abs((spr->h_move) ? spr->offset_x : spr->offset_y);

    if(pos_x <= 0 && offset < SHIFT_TILE)
        walking = false;

    if (pos_x >= MAXMAP_UNITS_X && offset < SHIFT_TILE)
        walking = false;

    if (pos_y <= 0 && offset < SHIFT_TILE)
        walking = false;

    if (pos_y >= MAXMAP_UNITS_Y && offset < SHIFT_TILE)
        walking = false;

    if (
        offset < SHIFT_TILE &&
        map->rockmap[pos_x][pos_y] != GROUND_BONE &&
        map->rockmap[pos_x][pos_y] != GROUND &&
        (map->rockmap[pos_x][pos_y] != ignore_object || map->rockmap[pos_x][pos_y] == BRICKS)
    )
        walking = false;

    if (
        spr->type_object == DEMONIETE &&
        offset < SHIFT_TILE &&
        (map->rockmap[pos_x][pos_y + 1] == GROUND_BONE || map->rockmap[pos_x][pos_y + 1] == GROUND)
    )
        walking = false;

    if (spr->type_object == ROCCO && !walking && map->rockmap[pos_x][pos_y] == OPEN_DOOR)
        map->level ++;

    if (!walking)
        return walking;

    collision.object_type   = OBJECT_STATIC;
    collision.ignore_object = ignore_object;
    collision_sprite(spr, map, &collision);
    walking = !collision.collision;

    if (!walking)
        return walking;

    if (spr->type_object != ROCCO) {

        collision.ignore_object = ignore_object;
        collision.object_type   = OBJECT_SPRITE;
        collision_sprite(spr, map, &collision);
        walking = !collision.collision;

        if (collision.object && collision.object->type_object == ROCCO) {
            switch (spr->type_object) {
                case ROCK:
                    spr->is_moving = false;
                case BOMB:
                case DIAMOND:
                    walking = false;
                    break;

                default:
                    sprites_death(map, collision.object, true);
                    walking = true;
                    break;
            }
        }
    }

    return walking;
}

/**
 * @brief Mueve el objeto que es empujado
 * @param *map Puntero al mapa
 * @param *object Puntero al objecto empujado
 */

void sprites_pushed_move(Rock_Scroll_Map *map, Rock_Sprite *object)
{
  int16_t move_x, move_y;
  bool has_ground;

    if (!object->active || object->current_pause)
        return;

    if (object->wait > 0) {
        object->wait --;
        return;
    }

    if (!object->h_move)
        return;

    move_x        = object->x_map + object->h_move;
    move_y        = object->y_map + 1;
    has_ground    = (map->objects_map[object->x_map][move_y] == EMPTY &&
                        (
                            map->rockmap[object->x_map][move_y] == GROUND ||
                            map->rockmap[object->x_map][move_y] == GROUND_BONE
                        )
                     ) ? true : false;

    if (sprites_can_move(move_x, object->y_map, object, map, 0)) {
        object->is_moving = true;
        sprites_move(object, map);
    } else {
        object->is_moving = false;
        object->h_move    = 0;
        object->move      = sprites_falls;
        object->is_push   = false;
    }

    if (has_ground) {
        object->offset_x += object->h_move * 2 * SHIFT_TILE;
        object->h_move    = 0;
        object->move      = sprites_falls;
    }
}

/**
 * @brief Deja de empujar
 * @param *map Puntero al mapa
 */

void sprites_push_off(Rock_Scroll_Map *map)
{
  Rock_Sprite *spr;
  int16_t index;

    spr = sprites;

    for (index = 0; index < map->objects; index ++) {
        if (spr->is_push) {
            spr->is_moving = false;
            spr->h_move    = 0;
            spr->move      = sprites_falls;
            spr->is_push   = false;
        }
        spr ++;
    }
}

/**
 * @brief Inicializa la animacón sanguinolenta y desactiva el sprite muerto
 * @param *map Puntero al mapa
 * @param *spr Puntero al sprite que muere
 * @param blood Determina si se aplica el efecto de animación o simplemente se marca como muerto
 */

void sprites_death(Rock_Scroll_Map *map, Rock_Sprite *spr, bool blood)
{
  int16_t offset_x, offset_y, death;

    death = (spr->type_object == ROCCO) ? ROCCO_DEATH : BLOOD;

    if (blood) {
        if (spr->is_visible)
            sfx_play(SFX_DEATH);

        spr->is_visible = false;

        offset_x = spr->offset_x;
        offset_y = spr->offset_y;

        map->deaths[spr->x_map][spr->y_map] = map->objects_map[spr->x_map][spr->y_map];
        sprites_define_object(spr, spr->x_map, spr->y_map, death);

        spr->offset_x = offset_x;
        spr->offset_y = offset_y;
    }

    map->objects_map[spr->x_map][spr->y_map] = EMPTY;

}

/**
 * @brief Disparador que se inicializa terminada la animación sanguinolenta. Marca la posición del sprite como vacía
 * @param *map Puntero al mapa
 * @param *spr Puntero al sprite
 */

void sprites_death_end(Rock_Scroll_Map *map, Rock_Sprite *spr)
{

    if (sprites_destroy(map, spr)) {
        if (spr->type_object == ROCCO_DEATH)
            map->rocco_death = true;

        map->deaths[spr->x_map][spr->y_map] = EMPTY;
    }

}

/**
 * @brief Desactiva el sprite en el caso de estar muerto
 * @param *map Puntero a la estructura de tipo Rock_Scroll_Map
 * @param *spr Puntero al objeto que hay que desactivar
 */

bool sprites_destroy(Rock_Scroll_Map *map, Rock_Sprite *spr)
{
  bool ends_animation = false;

    ends_animation = (spr->current_step == spr->steps_next_frame && (spr->current_frame + 1) >= spr->num_frames);

    if (
        (spr->x_map + 1) < (map->x - MAP_WIDTH) ||
        (spr->x_map - 1) > map->x ||
        (spr->y_map + 1) < (map->y - MAP_HEIGHT) ||
        (spr->y_map - 1) > map->y
    )
        ends_animation = true;

    if (ends_animation) {
        spr->active    = false;
        spr->is_moving = true;
        spr->frames    = NULL;
    }

    return ends_animation;
}

/**
 * @brief Sobredimensiona la memoria asignada a los objetos para poder albergar los nuevos diamantes a partir del sprite fly
 * @param new_index El número de índices nuevos para reasignar
 */

void sprites_add(int16_t new_index)
{
  Rock_Sprite *new;

    new = (Rock_Sprite *) realloc(sprites, new_index * sizeof(Rock_Sprite));

    if (new == NULL) {
        puts("No se han podido reservar los sprites nuevos en memoria");
        exit(EXIT_FAILURE);
    } else
        sprites = new;

}