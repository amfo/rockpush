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

#include <math.h>

#ifndef _COLLISION_
#define _COLLISION_

typedef struct Perimeter {
    int16_t from_x;
    int16_t from_y;
    int16_t to_x;
    int16_t to_y;
} Perimeter;

#endif

void collision_sprite(Rock_Sprite *, Rock_Scroll_Map *, Collision_Object *);
uint8_t collision_map(Rock_Scroll_Map *, uint8_t, uint8_t, uint8_t);
void collision_set_perimeter(Rock_Sprite *, Rock_Scroll_Map *, Perimeter *, int16_t);