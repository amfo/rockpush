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

#include "sfx.h"

static bool mixer_set = false;
static char music_files[15][25] = {
    "tolittle.mod",
    "avernus.mod",
    "b21990.xm",
    "bladswed.mod",
    "cloud.mod",
    "iamback.mod",
    "just_rocknroll.xm",
    "k-dust.s3m",
    "nz_blip.xm",
    "z-tower.xm",
    "justifie.mod",
    "moscow.mod",
    "70s_repr.mod",
    "tolittle.mod",
    "jellybean_holly_shit.mod"
};

Mix_Chunk *sfx_chunks[SFX_FALL + 1];

void sfx_init(void)
{
    if(!Mix_OpenAudio(FRECUENCY, AUDIO_S16SYS, CHANNELS, CHUNKSIZE)) {
        Mix_AllocateChannels(SFX_CHANNELS);
        Mix_VolumeMusic(MUSIC_VOLUME);

        mixer_set = true;
    }

    sfx_load_chunks();
}

void sfx_play(int action)
{
    if (action <= SFX_FALL)
        Mix_PlayChannel(-1, sfx_chunks[action], 0);
}

void sfx_load_chunks(void)
{
  int8_t i;
  char file[120];
  static char chunk_files[SFX_FALL + 1][25] = {
      "puerta.wav",
      "nivel.wav",
      "muerte.wav",
      "moneda.wav",
      "hierba.wav",
      "explosion.wav",
      "diamante.wav",
      "caida.wav"
  };


    for (i = 0; i < SFX_FALL  + 1; i ++) {
        sprintf(file, "%s%s", SFX_PATH, chunk_files[i]);

        if (!mixer_set)
            sfx_chunks[i] = NULL;
        else if ((sfx_chunks[i] = Mix_LoadWAV(file)) != NULL)
            Mix_VolumeChunk(sfx_chunks[i], SFX_VOUME);
    }
}

void sfx_close_chunks(void)
{
  int8_t i;

    for (i = 0; i < SFX_FALL + 1; i ++)
        Mix_FreeChunk(sfx_chunks[i]);
}

bool sfx_get_active(void)
{
    return mixer_set;
}

void sfx_set_level_music(Rock_Scroll_Map *map)
{
  char file[120];

    sprintf(file, "%s%s", MUSIC_PATH, music_files[map->level]);
    map->level_music = Mix_LoadMUS(file);
}
