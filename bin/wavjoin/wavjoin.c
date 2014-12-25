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
/*                1996-2014  Nagoya Institute of Technology          */
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

/************************************************************************
*                                                                       *
*    Make stereo WAV file by joining two monaural WAV files             *
*                                                                       *
*                                       2013.3 Akira Tamamori           *
*                                       2014.8 Takashi Aritake          *
*                                                                       *
*       usage:                                                          *
*               wavjoin [ options ]                                     *
*       options:                                                        *
*               -i I_LEFT I_RIGHT : Input WAV files or directories      *
*               -o O              : Output WAV file or directory        *
*       infile:                                                         *
*               wav file format                                         *
*       outfile:                                                        *      
*               wav file format                                         *
*                                                                       *
************************************************************************/

static char *rcs_id = "";

/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

#define STRLEN 512

/*  Command Name  */
char *cmnd;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr,
           " %s - Make stereo WAV file by joining two monaural WAV files \n",
           cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] \n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -i I  : Input WAV files or directories\n");
   fprintf(stderr, "       -o O  : Output WAV file or directory  \n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       wav file format \n");
   fprintf(stderr, "  outfile:\n");
   fprintf(stderr, "       wav file format \n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       wavjoin does not distinguish between\n");
   fprintf(stderr, "       small and capital letters of the file extension.\n");
   fprintf(stderr,
           "       The first input WAV file or directory is related to \n");
   fprintf(stderr,
           "       channel 0, and the other is related to channel 1.\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}


int main(int argc, char **argv)
{
   int i, j, idx;
   char *indir[2], *inname[2];
   for (i = 0; i < 2; i++) {
      indir[i] = NULL;
      inname[i] = NULL;
   }
   char *outdir = NULL, *outname = NULL;
   char strtmp[2][STRLEN];

   Filelist filelist[2];
   Wavfile wavin[2], wavout;

   if ((cmnd = strrchr(argv[0], '/')) == NULL) {
      cmnd = argv[0];
   } else {
      cmnd++;
   }
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'i':             /* input */
            separate_path(&indir[0], &inname[0], *++argv);
            --argc;
            if (argc > 1) {
               if (**(argv + 1) != '-') {
                  separate_path(&indir[1], &inname[1], *++argv);
                  --argc;
               }
            }
            break;
         case 'o':             /* output directory */
            /* single output */
            separate_path(&outdir, &outname, *++argv);
            --argc;
            break;
         case 'h':
            usage(EXIT_SUCCESS);
            break;
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(EXIT_FAILURE);
         }
      }
   }

   if (indir[0] == NULL || indir[1] == NULL) {
      fprintf(stderr, "%s : The pair of inputs must be specified!\n", cmnd);
      usage(EXIT_FAILURE);
   }
   if (outdir == NULL) {
      fprintf(stderr,
              "%s : The output wav file or directory must be specified!\n",
              cmnd);
      usage(EXIT_FAILURE);
   }
   if ((inname[0] == NULL) ^ (inname[1] == NULL)) {
      fprintf(stderr, "%s : The inputs must be either wavfile or directory!\n",
              cmnd);
      usage(EXIT_FAILURE);
   }
   if (inname[0] == NULL && outname != NULL) {
      fprintf(stderr,
              "%s : The output must be directory, if the inputs are directories!\n",
              cmnd);
      usage(EXIT_FAILURE);
   }

   if (inname[0] != NULL) {     /* inputs are pair of wav files */
      for (i = 0; i < 2; i++) {
         filelist[i].num = 1;
         filelist[i].name = (char **) getmem(1, sizeof(char *));
         filelist[i].name[0] = (char *) getmem(STRLEN, sizeof(char));
         strcpy(filelist[i].name[0], inname[i]);
      }
   } else {                     /* inputs are pair of directories */
      for (i = 0; i < 2; i++) {
         if (!get_wav_list(&filelist[i], indir[i])) {
            fprintf(stderr, "%s : Can't open directory %s!\n", cmnd, indir[i]);
            usage(EXIT_FAILURE);
         }
      }
   }

   for (i = 0; i < filelist[0].num; i++) {
      idx = 0;
      if (inname[0] != NULL) {
         for (j = 0; j < 2; j++) {
            sprintf(strtmp[j], "%s/%s", indir[j], filelist[j].name[i]);
         }
      } else {
         idx = search_wav_list(&filelist[1], filelist[0].name[i]);
         if (idx == -1) {
            fprintf(stderr, "%s wasn't matched to wav files in directory %s!\n",
                    filelist[0].name[i], indir[1]);
            continue;
         }
         sprintf(strtmp[0], "%s/%s", indir[0], filelist[0].name[i]);
         sprintf(strtmp[1], "%s/%s", indir[1], filelist[1].name[idx]);
      }

      /* Read WAV file */
      for (j = 0; j < 2; j++) {
         if (!wavread(&wavin[j], strtmp[j])) {
            fprintf(stderr, "%s : Can't open %s!\n", cmnd, strtmp[j]);
            usage(EXIT_FAILURE);
         }
         if (wavin[j].channel_num != 1) {
            fprintf(stderr, "%s isn't the monaural WAV file!\n", strtmp[j]);
         }
      }
      if (wavin[0].format_id != wavin[1].format_id) {
         fprintf(stderr, "%s : Inputs wav file format are mismatched!\n", cmnd);
         usage(EXIT_FAILURE);
      }
      if (wavin[0].sample_freq != wavin[1].sample_freq) {
         fprintf(stderr,
                 "%s : Inputs wav file sampling frequencies are mismatched!\n",
                 cmnd);
         usage(EXIT_FAILURE);
      }
      wavjoin(&wavout, wavin);

      if (outname == NULL) {
         if (inname[0] == NULL) {
            *(strrchr(filelist[0].name[i], '.')) = '\0';
            sprintf(strtmp[0], "%s/%s.wav", outdir, filelist[0].name[i]);
         } else {
            *(strrchr(filelist[0].name[i], '.')) = '\0';
            *(strrchr(filelist[1].name[i], '.')) = '\0';
            sprintf(strtmp[0], "%s/%s_%s.wav", outdir, filelist[0].name[i],
                    filelist[1].name[i]);
         }
      } else {
         sprintf(strtmp[0], "%s/%s", outdir, outname);
      }
      if (!wavwrite(&wavout, strtmp[0])) {
         fprintf(stderr, "%s : Can't open %s! \n", cmnd, strtmp[0]);
         usage(EXIT_FAILURE);
      }

      for (j = 0; j < 2; j++) {
         free_wav_data(&(wavin[j]));
      }
      free_wav_data(&wavout);

   }

   for (i = 0; i < 2; i++) {
      free_wav_list(&(filelist[i]));
   }

   return 0;
}
