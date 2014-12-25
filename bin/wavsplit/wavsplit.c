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
*    Split stereo WAV file into two monaural WAV files                  *
*                                                                       *
*                                       2013.3 Akira Tamamori           *
*                                       2014.8 Takashi Aritake          *
*                                                                       *
*       usage:                                                          *
*               wavsplit [ options ]                                    *
*       options:                                                        *
*               -i I : Input WAV file or directory                      *
*               -o O : Output WAV files or directories                  *
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
   fprintf(stderr, " %s - Split stereo WAV file into two monaural WAV files \n",
           cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] \n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -i I  : Input WAV file or directory\n");
   fprintf(stderr, "       -o O  : Output WAV files or directories\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       wav file format \n");
   fprintf(stderr, "  outfile:\n");
   fprintf(stderr, "       wav file format \n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       wavsplit does not distinguish between\n");
   fprintf(stderr, "       small and capital letters of the file extension.\n");
   fprintf(stderr,
           "       The first output WAV file or directory is related \n");
   fprintf(stderr,
           "       to channel 0, and the other is related to channel 1.\n");
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
   int i, j;
   char *indir = NULL, *inname = NULL, *tmp;
   char *outdir[2], *outname[2];
   for (i = 0; i < 2; i++) {
      outdir[i] = NULL;
      outname[i] = NULL;
   }
   char strtmp[STRLEN];

   Filelist filelist;
   Wavfile wavin, wavout[2];

   if ((cmnd = strrchr(argv[0], '/')) == NULL) {
      cmnd = argv[0];
   } else {
      cmnd++;
   }
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'i':             /* input */
            separate_path(&indir, &inname, *++argv);
            --argc;
            break;
         case 'o':             /* output directory */
            /* single output */
            separate_path(&outdir[0], &outname[0], *++argv);
            --argc;
            /* double output */
            if (argc > 1) {
               if (**(argv + 1) != '-') {
                  separate_path(&outdir[1], &outname[1], *++argv);
                  if ((outname[0] == NULL) ^ (outname[1] == NULL)) {
                     fprintf(stderr,
                             "%s : The outputs must be either wavfile or directory!\n",
                             cmnd);
                     usage(EXIT_FAILURE);
                  }
                  --argc;
               }
            }
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

   if (outname[0] != NULL && outname[1] == NULL) {
      fprintf(stderr, "%s : Output wav files must be pair! \n", cmnd);
      usage(EXIT_FAILURE);
   }
   if (indir == NULL) {
      fprintf(stderr, "%s : Input directory or wav file must be specified! \n",
              cmnd);
      usage(EXIT_FAILURE);
   }
   if (outdir[0] == NULL) {
      fprintf(stderr,
              "%s : The output wav file or directory must be specified! \n",
              cmnd);
      usage(EXIT_FAILURE);
   }
   if (inname == NULL) {
      if (!get_wav_list(&filelist, indir)) {
         fprintf(stderr, "%s : Can't open directory %s!\n", cmnd, indir);
         usage(EXIT_FAILURE);
      }
      if (outname[0] != NULL) {
         fprintf(stderr,
                 "%s : The outputs must be directory, if the input is directory!\n",
                 cmnd);
         usage(EXIT_FAILURE);
      }
   } else {
      filelist.num = 1;
      filelist.name = (char **) getmem(1, sizeof(char *));
      filelist.name[0] = (char *) getmem(STRLEN, sizeof(char));
      strcpy(filelist.name[0], inname);
   }

   for (i = 0; i < filelist.num; i++) {
      /* Read WAV file */
      sprintf(strtmp, "%s/%s", indir, filelist.name[i]);
      if (!wavread(&wavin, strtmp)) {
         fprintf(stderr, "%s : Can't open %s!\n", cmnd, strtmp);
         usage(EXIT_FAILURE);
      }
      if (wavin.channel_num != 2) {
         fprintf(stderr, "%s isn't the stereo WAV file! \n", strtmp);
         continue;
      }

      wavsplit(wavout, &wavin);
      for (j = 0; j < wavin.channel_num; j++) {
         if (outdir[1] == NULL) {       /* single output */
            tmp = strrchr(filelist.name[i], '.');
            if (j == 0 && tmp != NULL) {
               *tmp = '\0';
            }
            sprintf(strtmp, "%s/%s_%d.wav", outdir[0], filelist.name[i], j);
         } else {               /* double output */
            if (outname[0] == NULL && outname[1] == NULL) {
               sprintf(strtmp, "%s/%s", outdir[j], filelist.name[i]);
            } else {
               sprintf(strtmp, "%s/%s", outdir[j], outname[j]);
            }
         }
         if (!wavwrite(&(wavout[j]), strtmp)) {
            fprintf(stderr, "%s : Can't open %s! \n", cmnd, strtmp);
            usage(EXIT_FAILURE);
         }
      }

      free_wav_data(&wavin);
      for (j = 0; j < wavin.channel_num; j++) {
         free_wav_data(&(wavout[j]));
      }

   }

   free_wav_list(&filelist);
   return 0;
}
