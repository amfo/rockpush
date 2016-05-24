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
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_events.h>

#define SCREEN_TEXT_WIDTH       1024
#define SCREEN_TEXT_HEIGHT      160
#define TARGET_CHARACTER_WIDTH  64
#define TARGET_CHARACTER_HEIGHT 70
#define CHARACTER_WIDTH         20
#define CHARACTER_HEIGHT        22
#define NLETTERS                20
#define FONT_PATH               "images/font.png"
#define TINY_FONT_PATH          "images/font_tiny.png"

typedef struct
{
    short xpos;
    Uint16 sin_index;
    Uint16 font_pos;
} LETTER;

LETTER letters[NLETTERS];

Uint16 gfx_compute_font_pos(char);
Uint16 gfx_init_character(void);
void gfx_init(void);
void gfx_check_gpu(SDL_Renderer *);
short gfx_get_sin(short);
bool gfx_get_low_gpu(void);
void gfx_text_move(SDL_Renderer *, SDL_Texture *);