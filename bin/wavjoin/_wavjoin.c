/* ----------------------------------------------------------------- */
/*             The Speech Signal Processing Toolkit (SPTK)           */
/*             developed by SPTK Working Group                       */
/*             http://sp-tk.sourceforge.net/                         */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 1984-2007  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/*                1996-2016  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the SPTK working group nor the names of its */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/***************************************************************
    $Id: _wavjoin.c,v 1.4 2016/12/22 10:53:14 fjst15124 Exp $

    Split stereo WAV file in two monaural WAV files

        void   wavjoin(*wavout, *wavin)

        Wavfile wavout : split wav files
        Wavfile wavin : input wav file

***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

#define STRLEN 512

int search_wav_list(Filelist * filelist, char *key)
{
   int i;
   char *tmp;
   char strtmp0[STRLEN], strtmp1[STRLEN];
   strcpy(strtmp1, key);
   *(strrchr(strtmp1, '.')) = '\0';
   for (i = 0; i < filelist->num; i++) {
      strcpy(strtmp0, filelist->name[i]);
      tmp = strrchr(strtmp0, '.');
      if (tmp != NULL) {
         *tmp = '\0';
      }
      if (strcmp(strtmp0, strtmp1) == 0) {
         return i;
      }
   }

   return -1;
}

void wavjoin(Wavfile * wavout, const Wavfile * wavin)
{
   int i, j, len, byte_per_sample, data_num;

   if (wavin[0].data_chunk_size >= wavin[1].data_chunk_size) {
      len = wavin[0].data_chunk_size;
   } else {
      len = wavin[1].data_chunk_size;
   }

   /* WAV header */
   copy_wav_header(wavout, &wavin[0]);
   wavout->channel_num = 2;
   wavout->byte_per_sec = wavin->byte_per_sec * wavout->channel_num;
   wavout->block_size = wavin->block_size * wavout->channel_num;
   wavout->data_chunk_size = len * wavout->channel_num;
   wavout->file_size =
       4 * 3 + (4 * 2 + wavout->fmt_chunk_size) + (4 * 2 +
                                                   wavout->data_chunk_size) - 8;

   /* WAV waveform */
   byte_per_sample = wavout->bit_per_sample / 8;
   data_num = wavout->data_chunk_size / byte_per_sample;
   wavout->data = (char *) getmem(data_num * 2, byte_per_sample);
   for (i = 0; i < 2; i++) {
      byte_per_sample = wavin[i].bit_per_sample / 8;
      data_num = wavin[i].data_chunk_size / byte_per_sample;
      for (j = 0; j < data_num; j++) {
         memcpy(wavout->data + (2 * j + i) * byte_per_sample,
                wavin[i].data + j * byte_per_sample, byte_per_sample);
      }
   }

}
