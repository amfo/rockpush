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
#include "rockpush.h"
#include "sprites.h"
#include "collision.h"

void init_rocco_positions(uint8_t, uint8_t *, uint8_t *);
void rocco_move(Rock_Scroll_Map *, Rock_Sprite *);
void rocco_set_action(Rock_Sprite *, uint16_t);
bool rocco_can_move(Rock_Sprite *, Rock_Scroll_Map *);
uint8_t rocco_get_orientation(Rock_Sprite *);
bool rocco_edge(Rock_Scroll_Map *, Rock_Sprite *);
void rocco_push_rock(Rock_Sprite *, Rock_Sprite *, Rock_Scroll_Map *);
void rocco_takes(Rock_Sprite *, Rock_Sprite *, Rock_Scroll_Map *);
void rocco_burned(Rock_Scroll_Map *, Rock_Sprite *);