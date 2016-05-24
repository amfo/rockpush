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
#include "gfx.h"

/*
 * Efecto basado en "The_Demo_Effects_Collection"
 * http://demo-effects.sourceforge.net/
 */

static short gfx_sin[540];
static char text[] = " \
(C) 2016 AMFO Soft. \
Usa las teclas del cursor para desplazarte. \
Usa la tecla R para reiniciar el nivel\
La tecla de Escape sale del juego\
*** La mafia que controla la mina te explota hasta el punto que no te permitira salir de la mina hasta recoger todos los diamantes ***\"";
static char characters[] = " !##$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
static char *text_pointer = text;
static int gfx_character_width  = TARGET_CHARACTER_WIDTH;
static int gfx_character_height = TARGET_CHARACTER_HEIGHT;
static bool gfx_low_gpu = false;

Uint16 gfx_compute_font_pos(char scroll_char)
{
  char *p = characters;
  Uint16 pos = 0;

    if (scroll_char == '"') {
        text_pointer = text;
        scroll_char  = *text_pointer++;
    }

    while (*p++ != scroll_char) {
        pos += gfx_character_width;
    }

    if (pos > 0)
        return pos - 1;

    return 0;
}

Uint16 gfx_init_character(void)
{
    /* determine font character according to position in scroll text */

    return gfx_compute_font_pos(*text_pointer ++);
}

void gfx_init(void)
{
  float rad;
  Uint16 i, j;
  short centery = SCREEN_TEXT_HEIGHT - 54;
  text_pointer = text;
    /*create sin lookup table */

    for (i = 0, j = 0; i < 180; i ++) {
        rad =  (float)j * 0.0174532;
        gfx_sin[i] = centery - (short)((sin(rad) * 25.0));

        if (!( (i + 1) % 2))
            j++;
    }
    for (i = 90, j = 90; i < 270; i ++) {
        rad = (float)j * 0.0174532;
        gfx_sin[i + 90] = centery - (short)((sin(rad) * 25.0));

        if (!( (i + 1) % 2))
            j++;
    }

    for (i = 180; i < 270; i ++) {
        rad = (float)i * 0.0174532;
        gfx_sin[i + 180] = centery - (short)((sin(rad) * 25.0));
    }

    for (i = 270; i < 360; i++) {
        rad = (float)i * 0.0174532;
        gfx_sin[i + 180] = centery - (short)((sin(rad) * 25.0));
    }

    /* reset letters */
    for (i = 0; i < NLETTERS; ++ i) {
        letters[i].xpos = -gfx_character_width;
    }
}

void gfx_check_gpu(SDL_Renderer *screen)
{
  SDL_RendererInfo render_info;

    SDL_GetRendererInfo(screen, &render_info);
    if (render_info.max_texture_width < 8192) {
        gfx_low_gpu = true;
        gfx_character_width  = CHARACTER_WIDTH;
        gfx_character_height = CHARACTER_HEIGHT;
    }

}

short gfx_get_sin(short index)
{
    return gfx_sin[index];
}

bool gfx_get_low_gpu(void)
{
    return gfx_low_gpu;
}

void gfx_text_move(SDL_Renderer *screen, SDL_Texture *font_surface)
{
  static uint32_t displacement = 0;
  static SDL_Rect frect = {0, 0, TARGET_CHARACTER_WIDTH, TARGET_CHARACTER_HEIGHT};
  static SDL_Rect srect = {0, 0, TARGET_CHARACTER_WIDTH, TARGET_CHARACTER_HEIGHT};
  uint32_t i;
  int8_t diff = 0;

    if (gfx_low_gpu) {
        frect.w = gfx_character_width;
        frect.h = gfx_character_height;
    }

  /* Desplazamiento de texto */
    if (displacement > 40) {
        /* init new character */

        for (i = 0; i < NLETTERS; ++ i) {
            /* find an unused letter */

            if (letters[i].xpos < -TARGET_CHARACTER_WIDTH) {
                letters[i].xpos      = SCREEN_TEXT_WIDTH;
                letters[i].sin_index = 0;
                letters[i].font_pos  = gfx_init_character();
                break;
            }
        }

        displacement = 0;
    }

    displacement += 2;

    for (i = 0; i < NLETTERS; ++i) {
        letters[i].xpos -= 3;

        if (letters[i].xpos > -TARGET_CHARACTER_WIDTH) {
            letters[i].sin_index += 8;
            letters[i].sin_index %= 540;

            /* blit letter to screen */

            frect.x = letters[i].font_pos - 1;
            frect.w = gfx_character_width;
            srect.x = letters[i].xpos;

            srect.y = 560 + gfx_get_sin(letters[i].sin_index);

            SDL_RenderCopy(screen, font_surface, &frect, &srect);
        }
    }

}