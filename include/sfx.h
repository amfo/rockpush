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
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>
#include "rockpush.h"

#define MUSIC_PATH      "resources/music/"
#define SFX_PATH      "resources/sfx/"
#define FRECUENCY       22050
#define CHANNELS        2
#define CHUNKSIZE       4096
#define SFX_CHANNELS    30
#define SFX_VOUME       35
#define MUSIC_VOLUME    20

void sfx_init(void);
bool sfx_get_active(void);
void sfx_set_level_music(Rock_Scroll_Map *);
void sfx_play(int);
void sfx_load_chunks(void);
void sfx_close_chunks(void);