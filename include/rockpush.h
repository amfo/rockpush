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

#include <SDL2/SDL.h>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef GNAME

#define GNAME           "Rock Push"
#define TOTAL_LEVELS    15
#define MAXMAP_UNITS_X  50
#define MAXMAP_UNITS_Y  50
#define MAP_MASK_X      51
#define MAP_MASK_Y      51
#define MAXMAP          MAXMAP_UNITS_X * MAXMAP_UNITS_Y
#define MAXMAP_MASK     MAP_MASK_X * MAP_MASK_Y
#define MAP_WIDTH       15
#define MAP_HEIGHT      9
#define MINI_WIDTH      7
#define MINI_HEIGHT     4
#define SCROLL_W        8
#define SCROLL_H        5
#define VIEW            MAP_WIDTH * MAP_HEIGHT
#define SCROLL_SHIFT    6
#define INIT_ROCCO_LIVE 5

#define SURFACE         "images/sprites.png"
#define BACKGROUND      "images/rock_background.png"
#define FONT_TTF        "resources/ttf/jfrocsol.ttf"
#define MENU_TTF        "resources/ttf/rockin_record_g.ttf"
#define FRAME           struct frame
#define TILE_SIZE       64
#define TILE_MASK       TILE_SIZE - 1
#define TILE_DISTANCE   TILE_SIZE * TILE_SIZE
#define R_COLLISION     29
#define TILE_HALF       TILE_SIZE / 2
#define NEXT_CELL       TILE_SIZE / 4 * 3
#define SHIFT_TILE      6
#define ROCK_FRAMES     6
#define COLLISION       2
#define OBJECT_SPRITE   1
#define OBJECT_STATIC   2
#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   768
#define SCORE_Y         640
#define SCORE_X         55
#define SCORE_WIDTH     916
#define SCORE_HEIGHT    96
#define VIEW_WIDTH      MAP_WIDTH * TILE_SIZE
#define VIEW_HEIGHT     MAP_HEIGHT * TILE_SIZE
#define EDGE_WIDTH      ((MAXMAP_UNITS_X + 1) * TILE_SIZE) - ((MAP_WIDTH + 2) * TILE_SIZE)
#define EDGE_HEIGHT     ((MAXMAP_UNITS_Y + 1) * TILE_SIZE) - ((MAP_HEIGHT + 2) * TILE_SIZE)
#define BORDER          32
#define BPP             32
#define EMPTY           -1
#define RESERVED        -2

#define TICK_RATE       30
#define TICK_CREDITS    10
#define FALL_LEFT       4
#define FALL_RIGHT      5
#define EXPLOSION_SIZE  3
#define DISTORSION_LEVEL 40
#define CREDITS_SIZE    35
#define CREDITS_LINES   48
#define MENU_MUSIC      "resources/music/mainmenu.mod"
#define CREDITS_MUSIC   "resources/music/jellybean_cats_eye.mod"
#define SFX_FADE_OUT    600

typedef struct {
    int16_t view_width;
    int16_t view_height;
    int8_t scroll_shift;
    int16_t tmp_y;
    int16_t tmp_x;
    uint8_t x;
    uint8_t y;
    int16_t screen_x;
    int16_t screen_y;
    uint8_t diamonds;
    int16_t objects;
    int16_t level;
    uint8_t rockmap[MAP_MASK_X][MAP_MASK_Y];
    int16_t deaths[MAP_MASK_X][MAP_MASK_Y]; // Es como otra dimensión para evitar que interactúen los objetos
    int16_t objects_map[MAP_MASK_X][MAP_MASK_Y]; /* Posiciones de los objetos en un mapa relativo */
    int16_t rocco_index;
    int16_t points;
    uint8_t lives;
    bool    rocco_death;
    bool    refresh_rocco;
    bool    update_score;
    Mix_Music *level_music;
} Rock_Scroll_Map;

enum tile_type {
    GROUND,
    DIAMOND,
    GRASS,
    BRICKS,
    GUZZLER,
    DUMMY,
    DEMONIETE,
    FLY,
    ROCK,
    BLOOD,
    ROCCO_DEATH,
    ROCCO_BURNED,
    COIN = 14,
    BOMB,
    EXPLOSION,
    GROUND_BONE,
    DOOR,
    OPEN_DOOR,
    ROCCO = 99};
enum frame_type {WAIT_LEFT, WAIT_RIGHT, LEFT, RIGHT, DOWN, UP, PUSH_LEFT, PUSH_RIGHT};

enum sfx_types {
    SFX_DOOR,
    SFX_LEVEL,
    SFX_DEATH,
    SFX_COIN,
    SFX_GRASS,
    SFX_EXPLOSION,
    SFX_DIAMOND,
    SFX_FALL
};

FRAME {
    SDL_Rect size;
    FRAME *next_frame;
};

typedef struct Rock_Sprite Rock_Sprite;
struct Rock_Sprite {
    uint8_t x_map;
    uint8_t y_map;
    int16_t offset_x; /* Diferencia entre la representación del mapa en pantalla y la posición "real" */
    int16_t offset_y;
    bool    is_moving;
    bool    is_visible;
    bool    active;
    bool    is_push;
    bool    is_slide;
    int8_t  h_move;
    int8_t  v_move;
    int16_t type_object;
    int16_t index;
    int8_t  current_frame;
    int8_t  num_frames;
    int16_t steps_next_frame;
    int16_t current_step;
    int16_t pause_loop_frame;
    int16_t current_pause;
    int16_t min_frames_move;
    int16_t wait;
    uint16_t collision_radius;
    FRAME *frames;
    void (* move)(Rock_Scroll_Map *, Rock_Sprite *);
    void (* change_direction)(Rock_Sprite *);
};

typedef struct {
    bool collision;
    uint8_t value_object_map;
    int8_t object_type;
    int8_t ignore_object;
    uint16_t radius;
    Rock_Sprite *object;
} Collision_Object;

typedef struct {
    SDL_Texture *buffer_texture;
    SDL_Surface *buffer_surface;
    SDL_Texture *sprites_texture;
    SDL_Surface *sprites_surface;
    SDL_Texture *dump_texture;
    SDL_Renderer *screen;
    bool blit_surface;
} Rock_Screen;

FRAME tiles[20]; /* Objetos diferentes */
FRAME demon[6];
FRAME fly[4];
FRAME guzzler[6];
FRAME rocco[8];

Rock_Sprite *sprites;
TTF_Font *font;
TTF_Font *tiny_font;

#endif

int8_t rockpush_menu(Rock_Screen *);
void rockpush_init_game(Rock_Screen *);
void rock_exit(void);
void free_data(void);
void set_position(Rock_Sprite *, SDL_Event, SDL_Joystick *);
void set_text(void);
void set_score(Rock_Scroll_Map *, Rock_Screen *);
Rock_Sprite *set_level_rocco(Rock_Scroll_Map *);
uint8_t menu_options(uint8_t, int16_t, SDL_Surface *, TTF_Font *);
void distorsion_in(Rock_Scroll_Map *, Rock_Screen *);
void distorsion_out(Rock_Scroll_Map *, Rock_Screen *);
void rockpush_credits(SDL_Renderer *);