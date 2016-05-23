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

SDL_Renderer *screen_init(SDL_Window *, bool);
void screen_dump_buffer(Rock_Screen *, int, int);
int screen_copy_texture(Rock_Screen *, SDL_Rect *, SDL_Rect *);
SDL_Texture *screen_make_buffers(SDL_Renderer *, int, int);
SDL_Surface *screen_make_surface(int, int);
void screen_set_mask(Uint32 *, Uint32 *, Uint32 *, Uint32 *);
void screen_show_background(SDL_Renderer *);
void screen_put_pixel(SDL_Surface *, int16_t, int16_t, Uint32);
Uint32 screen_get_pixel(SDL_Surface *, int16_t, int16_t);
bool screen_greyscale(SDL_Surface *);
void screen_pixel_distorsion(SDL_Surface *, uint8_t);