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
#include "screen.h"
#include "rockpush.h"

/**
 * @brief Establece el modo de vídeo
 * @param *sdl_window Puntero de tipo SDL_Window
 * @param full_screen Define si se iniciará a pantalla completa o en ventana OpenGL
 * @return devuelve un puntero de tipo SDL_Renderer
 */

SDL_Renderer *screen_init(SDL_Window *sdl_window, bool full_screen)
{
  SDL_Renderer *screen;
  int init;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se pudo inicializar SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (full_screen)
        init = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &sdl_window, &screen);
    else
        init = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL, &sdl_window, &screen);

    if (init < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se pudo establecer el modo de vídeo: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawColor(screen, 0, 0, 0, 255);
    SDL_RenderClear(screen);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_SetWindowTitle(sdl_window, GNAME);
    SDL_RenderPresent(screen);

    return screen;
}

/**
 * @brief Muestra en pantalla la información almacenada en una SDL_Surface o en una SDL_Texture
 * @param *screen_data un puntero a la estructura de tipo Rock_Screen. Ahí se definen el tipo de información a volcar
 * @param width el ancho del área a representar
 * @param height el alto del área a representar
 */

void screen_dump_buffer(Rock_Screen *screen_data, int width, int height)
{
  SDL_Rect source;
  SDL_Rect target;

    source.x = TILE_SIZE;
    source.y = TILE_SIZE;
    source.w = width;
    source.h = height;

    target.x = target.y = BORDER;
    target.w = VIEW_WIDTH;
    target.h = VIEW_HEIGHT;

    if (screen_data->blit_surface)
        SDL_UpdateTexture(screen_data->dump_texture, NULL, screen_data->buffer_surface->pixels, screen_data->buffer_surface->pitch);
    else if (SDL_SetRenderTarget(screen_data->screen, NULL) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se ha podido asignar el render al modo por defecto: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_RenderCopy(screen_data->screen, screen_data->dump_texture, &source, &target);
    SDL_RenderPresent(screen_data->screen);
}

/**
 * @brief "wrapper" que copia la información de los sprites dependiendo de si se quieren usar como SDL_Surface o de tipo texturas
 * @param *screen_data puntero a una estructura de tipo Rock_Screen
 * @param *src Puntero a estructura de tipo SDL_Rect que define las coordenadas de origen
 * @param *src Puntero a estructura de tipo SDL_Rect que define las coordenadas de destino
 */

int screen_copy_texture(Rock_Screen *screen_data, SDL_Rect *src, SDL_Rect *dst)
{
    return (screen_data->blit_surface) ?
            SDL_BlitSurface(screen_data->sprites_surface, src, screen_data->buffer_surface, dst) :
            SDL_RenderCopy(screen_data->screen, screen_data->sprites_texture, src, dst);
}

/**
 * @brief Crea un textura de tipo SDL_Texture y devuelve el puntero a ella
 * @param *screen Puntero de tipo SDL_Renderer
 * @param width Ancho de la textura a crear
 * @param height Alto de la textura a crear
 * @return Puntero a la textura creada
 */

SDL_Texture *screen_make_buffers(SDL_Renderer *screen, int width, int height)
{
  SDL_Texture *buffer;

    buffer = SDL_CreateTexture(screen, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if (buffer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "No se pudo crear el buffer de intercambio: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return buffer;
}

/**
 * @brief Crea una superficie de mapa de bits de tipo SDL_Surface y devuelve el puntero a ella
 * @param width Ancho de la superficie a crear
 * @param height Alto de la superficie a crear
 * @return Puntero a la superficie creada
 */

SDL_Surface *screen_make_surface(int width, int height)
{
  SDL_Surface *surface;
  Uint32 r, g ,b, a;

    screen_set_mask(&r, &g, &b, &a);
    surface = SDL_CreateRGBSurface(0, width, height, BPP, r, g, b, a);

    if (surface == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "No se pudo crear la superficie de texturas: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return surface;
}

/**
 * @brief Define las máscaras para los píxeles y el canal alfa según la arquitectura del procesador
 * @param Uint32 * Puntero a la variable para el rojo
 * @param Uint32 * Puntero a la variable para el verde
 * @param Uint32 * Puntero a la variable para el azul
 * @param Uint32 * Puntero a la variable para el canal alfa
 */

void screen_set_mask(Uint32 *rmask, Uint32 *gmask, Uint32 *bmask, Uint32 *amask)
{
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        *rmask = 0x000000ff;
        *gmask = 0x0000ff00;
        *bmask = 0x00ff0000;
        *amask = 0xff000000;
    } else {
        *rmask = 0xff000000;
        *gmask = 0x00ff0000;
        *bmask = 0x0000ff00;
        *amask = 0x000000ff;
    }
}

/**
 * @brief Representa en pantalla el marco decorativo del juego a partir de una imagen de disco
 * @param *screen Puntero de tipo SDL_Renderer
 */

void screen_show_background(SDL_Renderer *screen)
{
  SDL_Texture *background;

    if ((background = IMG_LoadTexture(screen, BACKGROUND)) == NULL) {
        printf("Archivo de fondo no encontrado %s\n", SDL_GetError());
    } else {
        SDL_RenderCopy(screen, background, NULL, NULL);
        SDL_RenderPresent(screen);
    }

}

/**
 * @brief Pinta un píxel en una superficie dada según las coordenadas
 * @param *surface Puntero a la superficie SDL_Surface donde se quiere representar el píxel
 * @param x Posición X del píxel
 * @param x Posición Y del píxel
 * @param pixel información del píxel
 */

void screen_put_pixel(SDL_Surface *surface, int16_t x, int16_t y, Uint32 pixel)
{
  int16_t bpp;

    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) < 0) {
            fprintf(stderr, "No se puede bloquear la pantalla: %s\n", SDL_GetError());
            return;
        }
    }

    bpp      = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }

    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
}

/**
 * @brief Obtiene la información de un píxel en una superficie dada según las coordenadas
 * @param *surface Puntero a la superficie SDL_Surface donde se quiere representar el píxel
 * @param x Posición X del píxel
 * @param x Posición Y del píxel
 * @return información del píxel
 */

Uint32 screen_get_pixel(SDL_Surface *surface, int16_t x, int16_t y)
{
  int bpp;

    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) < 0) {
            fprintf(stderr, "No se puede bloquear la pantalla: %s\n", SDL_GetError());
            return 0;
        }
    }

    bpp      = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);

    switch(bpp) {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;

        default:
            return 0;
    }
}

/*
 * No soporta canal alfa
 * https://dzone.com/articles/converting-an-image-to-grayscale-using-sdl2
 *
bool screen_greyscale(SDL_Surface *surface)
{
  int16_t x, y;
  Uint8 v, r, g, b;
  Uint32 pixel;
  Uint32 *pixels = (Uint32 *)surface->pixels;

    for (y = 0; y < surface->h; y ++) {
        for (x = 0; x < surface->w; x ++) {
            pixel = pixels[y * surface->w + x];
            r     = pixel >> 16 & 0xff;
            g     = pixel >> 8  & 0xff;
            b     = pixel & 0xff;

            v     = (r + g + b) / 3;
            pixel = (0xff << 24) | (v << 16) | (v << 8) | v;
            pixels[y * surface->w + x] = pixel;
        }
    }

}
*/

/**
 * @brief Convierte una imágen de tipo SDL_Surface a escala de grises conservando el canal alfa
 * @param *image Puntero a la superficie de tipo SDL_Surface que se pretende transformar
 */

bool screen_greyscale(SDL_Surface *image)
{
    // http://cboard.cprogramming.com/game-programming/99430-how-make-grayscale-filter.html

  int8_t *p1, *p2, *pixels = NULL;
  uint32_t bytes = image->w * image->h * 4;
  uint32_t i, col, a, b, size;

    if (!(pixels = (int8_t *)malloc( bytes )))
        return false;

    // copy image data to ram
    p1 = pixels;
    p2 = (int8_t *)image->pixels;

    for (i = 0; i < bytes; i ++, p1 ++, p2 ++)
        *p1 = *p2;

    // convert to greyscale
    p1 = pixels;
    p2 = pixels;
    size = bytes / 4;

    for (a = 0; a < size; a ++) {
        col = 0;
        for(b = 0; b < 3; b ++, p1 ++) {
            if (*p1 > col)
                col = *p1; //read colours
        }

        p1 ++; //skip alpha byte
        col /= 3;

        for (b = 0; b < 3; b ++, p2 ++)
            *p2 = col;  //write greyscale

        p2 ++;
    }

    // copy greyscale to image
    p1 = pixels;
    p2 = (int8_t *)image->pixels;

    for (i = 0; i < bytes; i ++, p1 ++, p2 ++)
        *p2 = *p1;

    free(pixels);

    return true;
}

/**
 * @brief Aplica un nivel de distorsión a una imagen de tipo SDL_Surface
 * @param *image Puntero de la imagen de tipo SDL_Surface donde se pretende realizar la transformación
 * @param level Nivel de distorsión
 */

void screen_pixel_distorsion(SDL_Surface *image, uint8_t level)
{
  int16_t x, y, tx, ty;
  int8_t fromto;
  Uint32 pixel;

    fromto = level / 2;

    for (y = 0; y < image->h; y += level) {
        for (x = 0; x < image->w; x += level) {
            pixel = screen_get_pixel(image, x, y);

            for (ty = -fromto; ty < fromto; ty ++) {
                for (tx = -fromto ; tx < fromto; tx ++) {

                    if (x + tx >= 0 && y + ty >= 0 && x + tx < image->w && y + ty < image->h)
                        screen_put_pixel(image, x + tx, y + ty, pixel);
                }
            }
        }
    }
}