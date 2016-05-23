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
#include "rockpush.h"
#include "screen.h"
#include "sprites.h"
#include "map.h"
#include "rocco.h"
#include "collision.h"
#include "gfx.h"
#include "sfx.h"

int main(int argc, char **argv)
{
  SDL_Window *window = NULL;
  Rock_Screen screen_data;
  bool done = false, full_screen = true;
  int8_t selected;

    atexit(rock_exit);
    set_text();
    sfx_init();

    //Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);
    //music = Mix_LoadMUS("resources/music/avernus.mod");
    //Mix_PlayMusic(music, -1);

    full_screen = !(argc > 1 && !strcmp(argv[1], "-w"));

    screen_data.screen         = screen_init(window, full_screen);
    screen_data.buffer_surface = screen_make_surface(SCREEN_WIDTH, SCREEN_HEIGHT); // Surface para la pantalla principal
    screen_data.buffer_texture = screen_make_buffers(screen_data.screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    screen_data.dump_texture   = screen_make_buffers(screen_data.screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    screen_data.blit_surface   = false; // Se indica si se usarán texturas en GPU o surfaces en CPU
    sprites_load_textures(&screen_data);

    SDL_ShowCursor(SDL_DISABLE);

    while (!done) {
        selected = rockpush_menu(&screen_data);

        switch (selected) {
            case 1:
                rockpush_init_game(&screen_data);
                break;

            case 2:
                rockpush_credits(screen_data.screen);
                break;

            case 0:
            case 3:
                done = true;
                break;
        }
    }

    SDL_DestroyTexture(screen_data.dump_texture);
    SDL_DestroyTexture(screen_data.buffer_texture);
    SDL_FreeSurface(screen_data.buffer_surface);
    sprites_free_textures(&screen_data);
    SDL_DestroyRenderer(screen_data.screen);
    SDL_DestroyWindow(window);
    //Mix_FreeMusic(music);
    SDL_ShowCursor(SDL_ENABLE);

    exit(EXIT_SUCCESS);
}

/**
 * @brief Muestra el menú de opciones principal junto a una animación de un mapa aleatorio y un scroll de texto
 * @param *screen_data Puntero a estructura de tipo Rock_Screen
 * @return un entero con la opción seleccionada
 */

int8_t rockpush_menu(Rock_Screen *screen_data)
{
  SDL_Event event;
  SDL_Surface *menu;
  SDL_Surface *sprite_textures;
  SDL_Texture *font_surface;
  Rock_Sprite *rocco;
  Rock_Scroll_Map map;
  Mix_Music *music;
  TTF_Font *menu_font;
  bool done = false;
  Uint32 init_ms;
  int16_t offset;
  int8_t orientation, total_options, option;
  Sint32 event_option;
  SDL_Joystick *joystick = NULL;
  Sint16 joy_event = 0, joyy = 0;

    if (SDL_NumJoysticks() >= 1) {
        joystick = SDL_JoystickOpen(0);
        SDL_JoystickEventState(SDL_ENABLE);
    }

    if ((menu_font = TTF_OpenFont(MENU_TTF, 34)) == NULL) {
        printf("Error %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    if ((font_surface = IMG_LoadTexture(screen_data->screen, FONT_PATH)) == NULL) {
        printf("La fuente bitmap no encontrada %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    menu            = screen_make_surface(SCREEN_WIDTH, SCREEN_HEIGHT);
    sprite_textures = screen_make_surface(screen_data->sprites_surface->w, screen_data->sprites_surface->h);

    map.view_height   = MINI_HEIGHT * TILE_SIZE;
    map.view_width    = MINI_WIDTH  * TILE_SIZE;
    map.scroll_shift  = SCROLL_SHIFT / 2;
    map.diamonds      = 0;
    map.level         = (rand() % 15);
    map.points        = 0;
    map.lives         = INIT_ROCCO_LIVE;
    map.update_score  = false;
    map.refresh_rocco = false;

    rocco = set_level_rocco(&map);
    rocco->active = false;
    map.objects_map[rocco->x_map][rocco->y_map] = EMPTY;
    map.rockmap[rocco->x_map][rocco->y_map]     = GRASS;
    sprites_set_tiles_textures();

    //Se hace una copia de las texturas cargadas para convertirlas a B/N
    SDL_BlitSurface(screen_data->sprites_surface, NULL, sprite_textures, NULL);
    screen_greyscale(screen_data->sprites_surface);

    orientation = (rand() % 4) + 2;
    option = offset = 0;
    total_options = menu_options(option, map.view_width, menu, menu_font);
    gfx_init();

    screen_data->blit_surface = true;

    SDL_SetRenderDrawColor(screen_data->screen, 0, 0, 0, 255);
    SDL_RenderClear(screen_data->screen);

    if (sfx_get_active()) {
        music = Mix_LoadMUS(MENU_MUSIC);
        Mix_PlayMusic(music, -1);
    }

    while (!done) {

        init_ms = SDL_GetTicks();

        map_show(screen_data, &map, false);
        map_move(&map, orientation);
        SDL_BlitSurface(menu, NULL, screen_data->buffer_surface, NULL);
        //sprites_update(&map);

        screen_dump_buffer(screen_data, map.view_width, map.view_height);
        offset ++;

        if (offset > 240) {
            orientation = (rand() % 4) + 2;
            offset = 0;
        }


        /* Desplazamiento de texto */
        gfx_text_move(screen_data->screen, font_surface);

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                option = 0;
                done   = true;
            }

            if (SDL_JoystickGetAttached(joystick))
                joy_event = SDL_JoystickGetAxis(joystick, 0) | SDL_JoystickGetAxis(joystick, 1);

            if (event.type == SDL_KEYDOWN || event.type == SDL_JOYBUTTONDOWN || joy_event) {

                event_option = event.key.keysym.sym;

                if (SDL_JoystickGetAttached(joystick)) {
                    joyy      = SDL_JoystickGetAxis(joystick, 1);
                    joy_event = SDL_JoystickGetAxis(joystick, 0) | joyy;

                    if (SDL_JoystickGetButton(joystick, 0))
                        event_option = SDLK_ESCAPE;

                    if (SDL_JoystickGetButton(joystick, 1))
                        event_option = SDLK_RETURN;

                    if (joyy < -10)
                        event_option = SDLK_UP;

                    if (joyy > 10)
                        event_option = SDLK_DOWN;
                }

                switch (event_option) {
                    case SDLK_UP:
                        if (option > 0)
                            option --;

                        break;

                    case SDLK_DOWN:
                        if (option < total_options)
                            option ++;
                        break;

                    case SDLK_ESCAPE:
                        option = -1;
                    case SDLK_RETURN:
                        done = true;
                        option ++;
                        break;
                }

                menu_options(option, map.view_width, menu, menu_font);
            }
        }

        while ((SDL_GetTicks() - init_ms) < TICK_RATE);

    }

    //Restauramos la copia
    SDL_BlitSurface(sprite_textures, NULL, screen_data->sprites_surface, NULL);

    SDL_SetRenderDrawColor(screen_data->screen, 0, 0, 0, 255);
    SDL_RenderClear(screen_data->screen);
    screen_data->blit_surface = false;

    SDL_FreeSurface(sprite_textures);
    SDL_DestroyTexture(font_surface);
    SDL_FreeSurface(menu);
    TTF_CloseFont(menu_font);

    if (SDL_JoystickGetAttached(joystick))
        SDL_JoystickClose(joystick);

    if (sfx_get_active()) {
        Mix_FadeOutMusic(SFX_FADE_OUT);
        while (Mix_PlayingMusic()) ;
        Mix_FreeMusic(music);
    }

    return option;
}

/**
 * @brief Inicia el juego desde el primer nivel. Inicializa el mapa, los sprites y los controles
 * @param *screen_data Puntero a estructura de tipo Rock_Screen
 */

void rockpush_init_game(Rock_Screen *screen_data)
{
  SDL_Event event;
  Rock_Sprite *object_rocco;
  Rock_Scroll_Map map;
  int16_t current_level;
  bool done = false;
  Uint32 init_ms;
  Uint32 move_ms;
  SDL_Joystick *joystick = NULL;
  Sint16 joy_event = 0;

    if (SDL_NumJoysticks() >= 1) {
        joystick = SDL_JoystickOpen(0);
        SDL_JoystickEventState(SDL_ENABLE);
    }

    map.view_height   = VIEW_HEIGHT;
    map.view_width    = VIEW_WIDTH;
    map.scroll_shift  = SCROLL_SHIFT;
    map.diamonds      = 0;
    map.level         = 0;
    map.points        = 0;
    map.lives         = INIT_ROCCO_LIVE;
    map.update_score  = false;
    map.refresh_rocco = false;
    map.rocco_death   = false;
    current_level     = map.level;

    screen_show_background(screen_data->screen);
    sprites_set_tiles_textures();
    object_rocco = set_level_rocco(&map);
    set_score(&map, screen_data);


    if (sfx_get_active()) {
        sfx_set_level_music(&map);
        Mix_PlayMusic(map.level_music, -1);
    }

    SDL_GL_SetSwapInterval(0);

    distorsion_in(&map, screen_data);

    while (!done) {

        init_ms = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
                done = true;

            if (SDL_JoystickGetAttached(joystick))
                joy_event = SDL_JoystickGetAxis(joystick, 0) | SDL_JoystickGetAxis(joystick, 1);

            while (event.type == SDL_KEYDOWN || event.type == SDL_JOYBUTTONDOWN || joy_event) {
                move_ms = SDL_GetTicks();

                if (SDL_JoystickGetAttached(joystick)) {
                    joy_event = SDL_JoystickGetAxis(joystick, 0) | SDL_JoystickGetAxis(joystick, 1);

                    if (SDL_JoystickGetButton(joystick, 0))
                        done = true;

                    if (SDL_JoystickGetButton(joystick, 1))
                        map.rocco_death = true;
                }

                /*if (event.key.keysym.sym == SDLK_n)
                    map.level ++;*/

                // Reinicia el nivel
                if (event.key.keysym.sym == SDLK_r)
                    map.rocco_death = true;

                set_position(object_rocco, event, joystick);
                sprites_update(&map);
                map_show(screen_data, &map, false);
                screen_dump_buffer(screen_data, map.view_width, map.view_height);

                SDL_PollEvent(&event);

                if (map.update_score) {
                    set_score(&map, screen_data);
                    map.update_score = false;
                }

                if (map.level > current_level && map.level < TOTAL_LEVELS) {

                    sfx_play(SFX_LEVEL);

                    if (sfx_get_active()) {
                        Mix_HaltMusic();
                        Mix_FreeMusic(map.level_music);
                        sfx_set_level_music(&map);
                        Mix_PlayMusic(map.level_music, -1);
                    }

                    distorsion_out(&map, screen_data);

                    free(sprites);
                    object_rocco  = set_level_rocco(&map);
                    current_level = map.level;
                    set_score(&map, screen_data);

                    distorsion_in(&map, screen_data);

                } else if (map.level >= TOTAL_LEVELS)
                    done = true;

                if (map.refresh_rocco) {
                    object_rocco      = sprite_get_object(map.rocco_index);
                    map.refresh_rocco = false;
                }

                while ((SDL_GetTicks() - move_ms) < TICK_RATE);

            }

            if (event.type == SDL_KEYUP || !joy_event) {
                rocco_set_action(object_rocco, WAIT_LEFT);
                sprites_push_off(&map);
            }

        }

        sprites_update(&map);
        map_show(screen_data, &map, false);
        screen_dump_buffer(screen_data, map.view_width, map.view_height);

        if (map.update_score) {
            set_score(&map, screen_data);
            map.update_score = false;
        }

        // Si se añaden más elementos al mapa (realloc), hay que reasignar el puntero del personaje
        if (map.refresh_rocco) {
            object_rocco      = sprite_get_object(map.rocco_index);
            map.refresh_rocco = false;
        }

        if (map.rocco_death) {
            map.lives --;

            if (map.lives <= 0)
                done = true;
            else {
                free(sprites);
                object_rocco  = set_level_rocco(&map);
                current_level = map.level;
                set_score(&map, screen_data);
                map.rocco_death = false;
            }
        }

        while ((SDL_GetTicks() - init_ms) < TICK_RATE);

    }

    if (sfx_get_active()) {
        Mix_HaltMusic();
        Mix_FreeMusic(map.level_music);
    }

    distorsion_out(&map, screen_data);

    SDL_GL_SetSwapInterval(0);

    if (SDL_JoystickGetAttached(joystick))
        SDL_JoystickClose(joystick);
}

/**
 * @brief Función que se usará en atexit para desactivar los elementos inicializados y liberar la memoria
 */

void rock_exit(void)
{
    IMG_Quit();
    TTF_CloseFont(font);
    TTF_CloseFont(tiny_font);
    TTF_Quit();
    SDL_Quit();
    Mix_CloseAudio();
    free_data();
    sfx_close_chunks();

    puts("Adiós");
}

/**
 * @brief Librea la memoria reservada a los fotogramas de los sprites
 */

void free_data(void)
{
    if (sprites != NULL)
        free(sprites);

    if (tiles[ROCK].next_frame != NULL)
        free(tiles[ROCK].next_frame);

    if (tiles[DIAMOND].next_frame != NULL)
        free(tiles[DIAMOND].next_frame);

    if (tiles[COIN].next_frame != NULL)
        free(tiles[COIN].next_frame);

    if (demon[LEFT].next_frame != NULL) {
        free(demon[LEFT].next_frame);
        free(demon[RIGHT].next_frame);
        free(demon[WAIT_LEFT].next_frame);
        free(demon[WAIT_RIGHT].next_frame);
    }

    if (tiles[DUMMY].next_frame != NULL)
        free(tiles[DUMMY].next_frame);

    if (fly[LEFT].next_frame != NULL) {
        free(fly[LEFT].next_frame);
        free(fly[RIGHT].next_frame);
    }

    if (guzzler[LEFT].next_frame != NULL) {
        free(guzzler[LEFT].next_frame);
        free(guzzler[RIGHT].next_frame);
        free(guzzler[UP].next_frame);
        free(guzzler[DOWN].next_frame);
    }

    if (rocco[LEFT].next_frame != NULL) {
        free(rocco[WAIT_LEFT].next_frame);
        free(rocco[WAIT_RIGHT].next_frame);
        free(rocco[LEFT].next_frame);
        free(rocco[RIGHT].next_frame);
        free(rocco[UP].next_frame);
        free(rocco[DOWN].next_frame);
        free(rocco[PUSH_LEFT].next_frame);
        free(rocco[PUSH_RIGHT].next_frame);
    }

    if (tiles[ROCCO_BURNED].next_frame != NULL)
        free(tiles[ROCCO_BURNED].next_frame);
}

/**
 * @brief Mueve el personaje según las acciones realizadas por teclado y/o Joysctick
 * @param *object_rocco puntero a una estructura de tipo Rock_Sprite que será la que tenga el personaje
 * @param even estructura de tipo SDL_Event donde se registran los eventos de los elementos de entrada
 * @param *joystick puntero a estructura de tipo SDL_Joystick donde se registran las acciones del joystick
 */

void set_position(Rock_Sprite *object_rocco, SDL_Event event, SDL_Joystick *joystick)
{
  Sint16 joyx = 0, joyy = 0;
  Sint32 orientation;

    if (!(orientation = event.key.keysym.sym) && SDL_JoystickGetAttached(joystick)) {
        SDL_JoystickUpdate();

        joyx = SDL_JoystickGetAxis(joystick, 0);
        joyy = SDL_JoystickGetAxis(joystick, 1);

        if (joyy < -10)
            orientation = SDLK_UP;

        if (joyy > 10)
            orientation = SDLK_DOWN;

        if (joyx < -10)
            orientation = SDLK_LEFT;

        if (joyx > 10)
            orientation = SDLK_RIGHT;
    }

    switch (orientation) {
        case SDLK_RIGHT:
            rocco_set_action(object_rocco, RIGHT);
            break;

        case SDLK_LEFT:
            rocco_set_action(object_rocco, LEFT);
            break;

        case SDLK_UP:
            rocco_set_action(object_rocco, UP);
            break;

        case SDLK_DOWN:
            rocco_set_action(object_rocco, DOWN);
            break;

    }
}

/**
 * @brief Carga la fuente por defecto en dos tamaños distintos. Se usarán principalmente en el panel informativo
 */

void set_text(void)
{
    if (!TTF_WasInit() && TTF_Init() != 0) {
        printf("Error TTF %s \n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    if ((font = TTF_OpenFont(FONT_TTF, 64)) == NULL) {
        printf("Error %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    if ((tiny_font = TTF_OpenFont(FONT_TTF, 34)) == NULL) {
        printf("Error %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Define o refresca la información en el panel informativo como las vidas, diamantes por recoger, puntos y el nivel actual
 * @param *map Puntero a estructura de tipo Rock_Scroll_Map
 * @param *screen_data Puntero a estructura de tipo Rock_Screen
 */

void set_score(Rock_Scroll_Map *map, Rock_Screen *screen_data)
{
  SDL_Rect screen_target;
  SDL_Rect target;
  SDL_Surface *text;
  SDL_Texture *text_texture;
  SDL_Color text_color = {200, 200, 200};
  SDL_Color bg_color   = {0, 0, 0};
  char score_text[25];
  uint8_t i;
  int w, h;

    screen_target.x = SCORE_X;
    screen_target.y = SCORE_Y;
    screen_target.w = SCORE_WIDTH;
    screen_target.h = SCORE_HEIGHT;

    SDL_RenderFillRect(screen_data->screen, &screen_target);

    for (i = 0; i < map->lives; i ++) {
        target.x = (i * TILE_SIZE) + TILE_HALF;
        target.y = SCORE_Y + ((SCORE_HEIGHT - TILE_SIZE) / 2);
        target.h = target.w = TILE_SIZE;
        SDL_RenderCopy(screen_data->screen, screen_data->sprites_texture, &rocco[WAIT_RIGHT].size, &target);
    }

    target.x = 5 * TILE_SIZE + TILE_HALF;
    target.y = SCORE_Y + ((SCORE_HEIGHT - TILE_SIZE) / 2);
    target.h = target.w = TILE_SIZE;
    SDL_RenderCopy(screen_data->screen, screen_data->sprites_texture, &tiles[DIAMOND].size, &target);

    sprintf(score_text, "%d ", map->diamonds);

    TTF_SizeText(font, score_text, &w, &h);
    target.x = 6 * TILE_SIZE + TILE_HALF;
    target.y = SCORE_Y + ((SCORE_HEIGHT - h) / 2);
    target.w = w;
    target.h = h;

    text = TTF_RenderText_Shaded(font, score_text, text_color, bg_color);
    text_texture = SDL_CreateTextureFromSurface(screen_data->screen, text);
    SDL_RenderCopy(screen_data->screen, text_texture, NULL, &target);
    SDL_FreeSurface(text);
    SDL_DestroyTexture(text_texture);

    sprintf(score_text, "PUNTOS %d ", map->points);

    TTF_SizeText(font, score_text, &w, &h);
    target.x = 10 * TILE_SIZE;
    target.y = SCORE_Y + 14;
    target.w = w >> 1;
    target.h = h >> 1;

    text = TTF_RenderText_Shaded(font, score_text, text_color, bg_color);
    text_texture = SDL_CreateTextureFromSurface(screen_data->screen, text);
    SDL_RenderCopy(screen_data->screen, text_texture, NULL, &target);
    SDL_FreeSurface(text);
    SDL_DestroyTexture(text_texture);

    sprintf(score_text, "NIVEL %d ", map->level + 1);

    TTF_SizeText(font, score_text, &w, &h);
    target.x = 10 * TILE_SIZE;
    target.y = SCORE_Y + 50;
    target.w = w >> 1;
    target.h = h >> 1;

    text = TTF_RenderText_Shaded(font, score_text, text_color, bg_color);
    text_texture = SDL_CreateTextureFromSurface(screen_data->screen, text);
    SDL_RenderCopy(screen_data->screen, text_texture, NULL, &target);
    SDL_FreeSurface(text);
    SDL_DestroyTexture(text_texture);
}

/**
 * @brief Define un nuevo nivel para el juego, cargándolo de disco y definiendo los personajes que contiene
 * @param *map Puntero a estructura de tipo Rock_Scroll_Map
 */

Rock_Sprite *set_level_rocco(Rock_Scroll_Map *map)
{
    map_load(map);
    sprites_set_objects(map);

    return sprite_get_object(map->rocco_index);
}

/**
 * @brief Dibuja las opciones del menú sobre la animación del mapa aleatorio
 * @param un entero de byte con la opción seleccionada para resaltarla
 * @param entero con el ancho del área a visualiza, no tiene por qué coincidir con el tamaño de pantalla
 * @param *buffer Puntero de tipo SDL_Surface donde se vocará la información de bitmap generada
 * @param *ttf_font Puntero a de tipo TTF_Font donde se encuentra la fuente previamente cargada
 * @return entero con la opción seleccionada
 */

 uint8_t menu_options(uint8_t selected_option, int16_t width, SDL_Surface *buffer, TTF_Font *ttf_font)
{
  SDL_Rect target;
  SDL_Surface *text;
  SDL_Color text_color     = {200, 200, 200};
  SDL_Color selected_color = {50, 150, 250};
  SDL_Color current_color  = {128, 128, 128};
  int w, h, i, y;
  char *title = GNAME;
  char menu_options[3][25] = {
      "JUGAR",
      "CREDITOS",
      "SALIR"
  };

    y = TILE_HALF * 5;

    SDL_FillRect(buffer, NULL, 0x000000);
    TTF_SizeText(font, title, &w, &h);
    target.x = (width + (TILE_SIZE << 1) - w) >> 1;
    target.y = TILE_SIZE;
    target.w = w;
    target.h = h;

    text = TTF_RenderText_Blended(font, title, current_color);
    SDL_BlitSurface(text, NULL, buffer, &target);
    SDL_FreeSurface(text);

    current_color = text_color;

    for (i = 0; i < 3; i ++) {
        current_color = (selected_option == i) ? selected_color : text_color;

        TTF_SizeText(ttf_font, menu_options[i], &w, &h);
        target.x = (width + (TILE_SIZE << 1) - w) >> 1;
        target.y = y + i * h;
        target.w = w;
        target.h = h;

        text = TTF_RenderText_Blended(ttf_font, menu_options[i], current_color);
        SDL_BlitSurface(text, NULL, buffer, &target);
        SDL_FreeSurface(text);
    }

    return i - 1;
}

/**
 * @brief Efecto de distorsión de píxeles de menor resolución a mejor resolución
 * @param *map Puntero a estructura de tipo Rock_Scroll_Map
 * @param *screen_data Puntero a estructura de tipo Rock_Screen
 */

void distorsion_in(Rock_Scroll_Map *map, Rock_Screen *screen_data)
{
  SDL_Surface *deform;
  SDL_Surface *buffer_backup;
  uint8_t distorsion;
  bool blit_status;

    deform      = screen_make_surface(SCREEN_WIDTH, SCREEN_HEIGHT); // para el efecto de transición
    blit_status = screen_data->blit_surface;
    screen_data->blit_surface = true;
    map_show(screen_data, map, false);
    buffer_backup = screen_data->buffer_surface;
    screen_data->buffer_surface = deform;

    for (distorsion = DISTORSION_LEVEL; distorsion > 0; distorsion --) {
        SDL_BlitSurface(buffer_backup, NULL, deform, NULL);
        screen_pixel_distorsion(deform, distorsion);
        screen_dump_buffer(screen_data, map->view_width, map->view_height);
    }

    screen_data->buffer_surface = buffer_backup;
    screen_data->blit_surface   = blit_status;
    SDL_FreeSurface(deform);
}

/**
 * @brief Efecto de distorsión de píxeles desde la resolución actual hasta un número finito
 * @param *map Puntero a estructura de tipo Rock_Scroll_Map
 * @param *screen_data Puntero a estructura de tipo Rock_Screen
 */

void distorsion_out(Rock_Scroll_Map *map, Rock_Screen *screen_data)
{
  uint8_t distorsion;
  bool blit_status;

    blit_status = screen_data->blit_surface;
    screen_data->blit_surface = true;
    map_show(screen_data, map, false);

    for (distorsion = 1; distorsion <= DISTORSION_LEVEL; distorsion ++) {
        screen_pixel_distorsion(screen_data->buffer_surface, distorsion);
        screen_dump_buffer(screen_data, map->view_width, map->view_height);
    }

    screen_data->blit_surface = blit_status;
}

/**
 * @brief Muestra los créditos del programa con un scroll vertical
 * @param *screen Puntero de tipo SDL_Renderer
 */

void rockpush_credits(SDL_Renderer *screen)
{
  SDL_Texture *text_texture;
  SDL_Surface *text;
  TTF_Font *credits_font;
  Mix_Music *music;
  SDL_Event event;
  SDL_Color text_color = {128, 128, 128};
  SDL_Color bg_color   = {0, 0, 0};
  SDL_Rect target;
  bool done = false;
  Uint32 init_ms;
  char screen_text[CREDITS_LINES][80] = {
      "DESARROLLO",
      "Antonio M. Fernandez Orts",
      "",
      "GRAFICOS",
      "Rocco -- Esther Company",
      "Dummy -- bevouliin -- opengameart.org",
      "Fly -- bevouliin -- opengameart.org",
      "Guzzler -- bevouliin -- opengameart.org",
      "Sangre -- PWL -- opengameart.org",
      "Demoniete -- upsidesmile -- www.shutterstock.com",
      "Piedra -- Memo Angeles -- www.shutterstock.com",
      "Diamantes -- Lilu330 -- www.shutterstock.com",
      "",
      "EFECTOS DE SONIDO",
      "caida.wav -- Gede",
      "diamante.wav -- Gede",
      "explosion.wav -- Gede",
      "hierba.wav -- Gede",
      "moneda.wav -- Gede",
      "muerte.wav -- Gede",
      "nivel.wav -- Gede",
      "puerta.wav -- Gede",
      "",
      "MUSICA",
      "tolittle.mod -- Gede",
      "moscow.mod -- Gede",
      "justifie.mod -- Gede",
      "jellybean_holly_shit.mod -- Gede",
      "jellybean_cats_eye.mod -- Gede",
      "70s_repr.mod -- audiomonster -- modarchive.org",
      "b21990.xm -- Phoenix -- modarchive.org",
      "bladswed.mod -- Allister Brimble -- modarchive.org",
      "cloud.mod -- Zodiak -- modarchive.org",
      "just_rocknroll.xm -- infra -- modarchive.org",
      "avernus.mod --  -- modarchive.org",
      "k-dust.s3m -- Leviathan -- modarchive.org",
      "z-tower.xm -- zealan -- modarchive.org",
      "nz_blip.xm -- nitzer -- modarchive.org",
      "mainmenu.mod --  -- modarchive.org",
      "iamback.mod --  -- modarchive.org",
      "",
      "FUENTES DE TEXTO",
      "Jester Font Studio (Rock Solid Font)",
      "Rockin Record",
      "Origami Mommy",
      "",
      "Mas informacion en CREDITOS.TXT"
  };
  int w, h, i;

  typedef struct {
    int pos_y;
    int text_line;
  } Credits;

    if (sfx_get_active()) {
        music = Mix_LoadMUS(CREDITS_MUSIC);
        Mix_PlayMusic(music, -1);
    }

    Credits credits[CREDITS_LINES];

    if ((credits_font = TTF_OpenFont(MENU_TTF, CREDITS_SIZE - 1)) == NULL) {
        printf("Error %s\n", TTF_GetError());
        return;
    }

    for (i = 0; i < CREDITS_LINES; i ++) {
        credits[i].pos_y = SCREEN_HEIGHT + i * CREDITS_SIZE;
        credits[i].text_line = i;
    }

    SDL_SetRenderDrawColor(screen, 0, 0, 0, 255);
    SDL_RenderClear(screen);

    while (!done) {
        init_ms = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
        }

        for (i = 0; i < CREDITS_LINES; i ++) {

            TTF_SizeText(credits_font, screen_text[credits[i].text_line], &w, &h);
            target.x = (SCREEN_WIDTH - w) >> 1;
            target.y = SCREEN_HEIGHT;
            target.w = w;
            target.h = h;

            if (credits[i].pos_y < SCREEN_HEIGHT)
                target.y = credits[i].pos_y;
            else if (credits[i].pos_y < 0) {
                target.y = 0;
                target.h = h - credits[i].pos_y;
            }

            if (target.y < SCREEN_HEIGHT && target.h > 0) {
                text = TTF_RenderText_Shaded(credits_font, screen_text[credits[i].text_line], text_color, bg_color);
                text_texture = SDL_CreateTextureFromSurface(screen, text);
                SDL_RenderCopy(screen, text_texture, NULL, &target);
                SDL_FreeSurface(text);
                SDL_DestroyTexture(text_texture);
            }

            credits[i].pos_y --;
        }

        if (credits[i - 1].pos_y <= -CREDITS_SIZE)
            done = true;

        SDL_RenderPresent(screen);
        while ((SDL_GetTicks() - init_ms) < TICK_CREDITS);
    }

    if (sfx_get_active()) {
        Mix_FadeOutMusic(SFX_FADE_OUT);
        while (Mix_PlayingMusic()) ;
        Mix_FreeMusic(music);
    }

    TTF_CloseFont(credits_font);
    SDL_SetRenderDrawColor(screen, 0, 0, 0, 255);
    SDL_RenderClear(screen);

}