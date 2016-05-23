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

void sprites_load_textures(Rock_Screen *);
void sprites_free_textures(Rock_Screen *);
void sprite_set_frames_to_sprite(FRAME *, int16_t, int16_t, int16_t, int16_t, int16_t);
void sprites_set_tile_ground(void);
void sprites_set_tile_grass(void);
void sprites_set_tile_rock(void);
void sprites_set_tile_door(void);
void sprites_set_tile_opendoor(void);
void sprites_set_tile_diamond(void);
void sprites_set_tile_bomb(void);
void sprites_set_demoniete(void);
void sprites_set_guzzler(void);
void sprites_set_fly(void);
void sprites_set_rocco(void);
void sprites_set_tiles_textures(void);
void sprites_set_sprite_from_map(Rock_Screen *, Rock_Scroll_Map *);
void sprites_set_tile_from_map(Rock_Screen *, Rock_Scroll_Map *);
void sprites_update(Rock_Scroll_Map *);
void sprites_set_objects(Rock_Scroll_Map *);
void sprites_define_object(Rock_Sprite *, int16_t, int16_t, int16_t);
Rock_Sprite *sprite_get_object(int16_t);
void sprite_get_current_frame(SDL_Rect *, Rock_Sprite *);
int16_t sprites_move(Rock_Sprite *, Rock_Scroll_Map *);
void sprites_shift(Rock_Sprite *, Rock_Scroll_Map *);
void sprites_falls(Rock_Scroll_Map *, Rock_Sprite *);
void sprites_slide(Rock_Scroll_Map *, Rock_Sprite *);
bool sprites_can_move(int16_t, int16_t, Rock_Sprite *, Rock_Scroll_Map *, int8_t);
void sprites_pushed_move(Rock_Scroll_Map *, Rock_Sprite *);
void sprites_push_off(Rock_Scroll_Map *);
void sprites_death(Rock_Scroll_Map *, Rock_Sprite *, bool);
void sprites_death_end(Rock_Scroll_Map *map, Rock_Sprite *spr);
bool sprites_destroy(Rock_Scroll_Map *, Rock_Sprite *);
void sprites_add(int16_t);