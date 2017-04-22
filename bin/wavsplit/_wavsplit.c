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
    $Id: _wavsplit.c,v 1.4 2016/12/22 10:53:14 fjst15124 Exp $

    Split stereo WAV file into two monaural WAV files

        void   wavsplit(*wavout, *wavin)

        Wavfile wavout : split wav files
        Wavfile wavin : input wav file

***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

#define STRLEN 512

void copy_wav_header(Wavfile * dest_wav, const Wavfile * source_wav);

void capital2small(char *s)
{
   char *p;
   for (p = s; *p; p++) {
      *p = tolower(*p);
   }
}

void separate_path(char **dir, char **name, char *path)
{
   char *tmp;
   tmp = strrchr(path, '.');
   if (tmp != NULL && *(tmp + 1) != '/' && *(tmp + 1) != '\0') {        /* input a file */
      tmp = strrchr(path, '/');
      if (tmp == NULL) {        /* WAV file name only */
         *dir = "./";
         *name = path;
      } else {                  /* directory and Wav file name */
         *name = tmp + 1;
         *tmp = '\0';
         *dir = path;
      }
   } else {                     /* input a directory */
      tmp = strrchr(path, '\0');
      if (*(tmp - 1) == '/') {
         *(tmp - 1) = '\0';
      }
      *dir = path;
      *name = NULL;
   }
}

Boolean get_wav_list(Filelist * filelist, const char *dirname)
{
   int idx = 0;
   char *tmp;
   char ctmp[STRLEN];
   DIR *pdir = NULL;
   struct dirent *ptr = NULL;

   filelist->num = 0;
   /* Open Directory */
   pdir = opendir(dirname);
   if (pdir == NULL) {
      filelist->num = -1;
      return 0;
   }

   /* Count WAV files */
   while ((ptr = readdir(pdir)) != NULL) {
      strcpy(ctmp, ptr->d_name);
      tmp = strrchr(ctmp, '.');
      if (tmp != NULL) {
         capital2small(tmp);
         if (strcmp(tmp, ".wav") == 0) {
            filelist->num++;
         }
      }
   }
   rewinddir(pdir);

   /* Making WAV files list */
   filelist->name = (char **) getmem(filelist->num, sizeof(char *));
   while ((ptr = readdir(pdir)) != NULL) {
      strcpy(ctmp, ptr->d_name);
      tmp = strrchr(ctmp, '.');
      if (tmp != NULL) {
         capital2small(tmp);
         if (strcmp(tmp, ".wav") == 0) {
            filelist->name[idx] =
                (char *) getmem(strlen(ptr->d_name) + 1, sizeof(char));
            strcpy(filelist->name[idx], ptr->d_name);
            idx++;
         }
      }
   }
   closedir(pdir);
   return 1;
}

Boolean wavread(Wavfile * wavfile, const char *fullpath)
{
   FILE *fp = NULL;
   char fmt_buf[STRLEN] = { '\0' };
   int rest_size = 0, current_position = 0, chunk_size = 0;
   /* Read WAV file */
   fp = fopen(fullpath, "rb");
   if (fp == NULL) {
      fprintf(stderr, "WARNING : Can't open wav file %s! \n", fullpath);
      return 0;
   }

   /* Read WAV header */
   if (freadx(fmt_buf, sizeof(char), 4, fp) < 4) {
      fprintf(stderr, "WARNING : RIFF is corrupted! \n");
      return 0;
   }
   if (strcmp(fmt_buf, "RIFF") != 0) {
      fprintf(stderr, "WARNING : Invalid WAV file! \n");
      return 0;
   }

   if (freadx(&(wavfile->file_size), 4, 1, fp) < 1) {
      fprintf(stderr, "WARNING : RIFF is corrupted! \n");
      return 0;
   }
   wavfile->file_size += 8;

   if (freadx(fmt_buf, sizeof(char), 4, fp) < 4) {
      fprintf(stderr, "WARNING : RIFF is corrupted! \n");
      return 0;
   }
   if (strcmp(fmt_buf, "WAVE") != 0) {
      fprintf(stderr, "WARNING : Invalid WAV file! \n");
      return 0;
   }

   rest_size = wavfile->file_size - 12;
   current_position = 12;

   while (rest_size > 0) {
      /* read chunk ID (4 byte) */
      if (freadx(fmt_buf, sizeof(char), 4, fp) < 4) {
         fprintf(stderr, "WARNING : Invalid chunk ID!\n");
         return 0;
      }
      current_position += 4;

      if (strcmp(fmt_buf, "fmt ") == 0) {       /* read chunk size (4 byte) */
         if (freadx(&(wavfile->fmt_chunk_size), 4, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid fmt chunk size!\n");
            return 0;
         }
         current_position += 4;

         if (freadx(&(wavfile->format_id), 2, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid format chunk!\n");
            return 0;
         }
         if (freadx(&(wavfile->channel_num), 2, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid channel number!\n");
            return 0;
         }
         if (freadx(&(wavfile->sample_freq), 4, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid sampling frequency!\n");
            return 0;
         }
         if (freadx(&(wavfile->byte_per_sec), 4, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid Byte/sec value!\n");
            return 0;
         }
         if (freadx(&(wavfile->block_size), 2, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid block size!\n");
            return 0;
         }
         if (freadx(&(wavfile->bit_per_sample), 2, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid Bit/sample value!\n");
            return 0;
         }
         if (wavfile->bit_per_sample == 8) {
            wavfile->input_data_type = 'c';
         } else if (wavfile->bit_per_sample == 16) {
            wavfile->input_data_type = 's';
         } else if (wavfile->bit_per_sample == 24) {
            wavfile->input_data_type = 'i';
         } else if (wavfile->bit_per_sample == 32) {
            if (wavfile->format_id == 1) {      /* PCM */
               wavfile->format_type = 0;
               wavfile->input_data_type = 'I';
            } else if (wavfile->format_id == 3) {       /* IEEE float */
               wavfile->format_type = 1;
               wavfile->input_data_type = 'f';
            }
         }
         current_position += wavfile->fmt_chunk_size;
         if (wavfile->fmt_chunk_size != 16) {
            if (freadx(&(wavfile->extended_size), 2, 1, fp) < 1) {
               fprintf(stderr, "WARNING : RIFF is corrupted.\n");
               return 0;
            }
            fseek(fp, current_position, wavfile->extended_size);
            current_position += wavfile->extended_size;
         }
      } else if (strcmp(fmt_buf, "data") == 0) {        /* read data chunk */
         if (freadx(&(wavfile->data_chunk_size), 4, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid data chunk size!\n");
            return 0;
         }
         current_position += 4;
         wavfile->data = (char *) getmem(wavfile->data_chunk_size, 1);
         if (freadx(wavfile->data, 1, wavfile->data_chunk_size, fp)
             < wavfile->data_chunk_size) {
            fprintf(stderr, "WARNING : Invalid waveform data!\n");
            return 0;
         }
         current_position += wavfile->data_chunk_size;
      } else {                  /* skip other chunks */
         if (freadx(&chunk_size, 4, 1, fp) < 1) {
            fprintf(stderr, "WARNING : Invalid chunk!\n");
            return 0;
         }
         current_position += 4;
         fseek(fp, current_position, chunk_size);
         current_position += chunk_size;
      }
      rest_size = wavfile->file_size - current_position;
   }

   fclose(fp);

   return 1;
}

Boolean wavwrite(Wavfile * wavfile, const char *outpath)
{
   FILE *fp = NULL;
   char tmp[STRLEN];
   int buf[2] = { 0, 0 };

   /* File Open */
   fp = fopen(outpath, "wb");
   if (fp == NULL) {
      return 0;
   }

   /* RIFF header */
   strcpy(tmp, "RIFF");
   fwritex(tmp, sizeof(char), 4, fp);
   /* file size */
   buf[0] = wavfile->file_size;
   fwrite_little_endian(buf, 4, 1, fp);
   /* WAVE format */
   strcpy(tmp, "WAVE");
   fwritex(tmp, 4, 1, fp);

   /* fmt chunk */
   strcpy(tmp, "fmt ");
   fwritex(tmp, 4, 1, fp);
   /* chunk size */
   buf[0] = wavfile->fmt_chunk_size;
   fwrite_little_endian(buf, 4, 1, fp);
   /* format_id */
   fwritex(&(wavfile->format_id), 2, 1, fp);
   /* channel_num (mono:1, stereo:2) */
   fwritex(&(wavfile->channel_num), 2, 1, fp);
   /* sampling frequency */
   buf[0] = wavfile->sample_freq;
   fwrite_little_endian(buf, 4, 1, fp);
   /* Byte/sec */
   buf[0] = wavfile->byte_per_sec;
   fwrite_little_endian(buf, 4, 1, fp);
   /* block size */
   fwritex(&(wavfile->block_size), 2, 1, fp);
   /* Bit/sample */
   fwritex(&(wavfile->bit_per_sample), 2, 1, fp);
   /* extension */
   if (wavfile->fmt_chunk_size != 16) {
      fwritex(&(wavfile->extended_size), 2, 1, fp);
   }

   /* 'data' chunk */
   strcpy(tmp, "data");
   fwritex(tmp, 4, 1, fp);
   /* data chunk size */
   buf[0] = wavfile->data_chunk_size;
   fwritex(buf, 4, 1, fp);
   /* waveform data */
   fwritex(wavfile->data, sizeof(char), wavfile->data_chunk_size, fp);

   fclose(fp);

   return 1;
}

void wavsplit(Wavfile * wavout, const Wavfile * wavin)
{
   int i, j, data_num, byte_per_sample;
   Wavfile *head = wavout;

   for (i = 0; i < wavin->channel_num; i++) {
      /* WAV header */
      copy_wav_header(wavout, wavin);
      wavout->channel_num = 1;
      wavout->byte_per_sec = wavin->byte_per_sec / wavin->channel_num;
      wavout->block_size = wavin->block_size / wavin->channel_num;
      wavout->data_chunk_size = wavin->data_chunk_size / wavin->channel_num;
      wavout->file_size =
          4 * 3 + (4 * 2 + wavout->fmt_chunk_size) + (4 * 2 +
                                                      wavout->data_chunk_size) -
          8;

      /* WAV waveform */
      byte_per_sample = wavout->bit_per_sample / 8;
      data_num = wavout->data_chunk_size / byte_per_sample;
      wavout->data = (char *) getmem(data_num, byte_per_sample);
      for (j = 0; j < data_num; j++) {
         memcpy(wavout->data + (j * byte_per_sample),
                wavin->data + (wavin->channel_num * j + i) * byte_per_sample,
                byte_per_sample);
      }

      wavout++;
   }
   wavout = head;

}

void copy_wav_header(Wavfile * dest_wav, const Wavfile * source_wav)
{
   dest_wav->file_size = source_wav->file_size;
   dest_wav->fmt_chunk_size = source_wav->fmt_chunk_size;
   dest_wav->data_chunk_size = source_wav->data_chunk_size;
   dest_wav->format_id = source_wav->format_id;
   dest_wav->channel_num = source_wav->channel_num;
   dest_wav->sample_freq = source_wav->sample_freq;
   dest_wav->byte_per_sec = source_wav->byte_per_sec;
   dest_wav->block_size = source_wav->block_size;
   dest_wav->bit_per_sample = source_wav->bit_per_sample;
   dest_wav->extended_size = source_wav->extended_size;
   dest_wav->input_data_type = source_wav->input_data_type;
   dest_wav->format_type = source_wav->format_type;
}

void free_wav_list(Filelist * filelist)
{
   int i;

   for (i = 0; i < filelist->num; i++) {
      free(filelist->name[i]);
   }
   free(filelist->name);
}

void free_wav_data(Wavfile * wavfile)
{
   free(wavfile->data);
}
