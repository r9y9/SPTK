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
/*                1996-2013  Nagoya Institute of Technology          */
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
*    Pitch Extraction                                                   *
*                                                                       *
*                                      1998.7  M.Tamura                 *
*                                      2000.3  T.Tanaka                 *
*                                      2011.10 A.Tamamori               *
*                                      2011.11 T.Sawada                 *
*                                                                       *
*       usage:                                                          *
*               pitch [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -a  a     :  algorithm used for pitch      [0]          *
*                            estimation                                 *
*                              0 (RAPT)                                 *
*                              1 (SWIPE')                               *
*               -s  s     :  sampling frequency (Hz)       [16]         *
*               -p  p     :  frame shift                   [80]         *
*               -T  T     :  voiced/unvoiced threshold     [0.0]        *
*                            (used only for RAPT algorithm)             *
*               -t  t     :  voiced/unvoiced threshold     [0.3]        *
*                            (used only for SWIPE' algorithm)           *
*               -L  L     :  minimum fundamental frequency [60]         *
*                            to search for (Hz)                         *
*               -H  H     :  maximum fundamental frequency [240]        *
*                            to search for (Hz)                         *
*               -o  o     :  output format                 [0]          *
*                              0 (pitch)                                *
*                              1 (f0)                                   *
*                              2 (log(f0))                              *
*       infile:                                                         *
*               data sequence                                           *
*                       x(0), x(1), ..., x(n-1), ...                    *
*       stdout:                                                         *
*               pitch, f0, or log(f0)                                   *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: pitch.c,v 1.45 2013/12/16 09:02:02 mataki Exp $";


/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

/*  Default Values  */
#define LOW    60.0
#define HIGH   240.0
#define FRAME_SHIFT 80
#define SAMPLE_FREQ 16.0
#define ATYPE 0
#define OTYPE 0
#define STR_LEN 255
#define THRESH_RAPT 0.0
#define THRESH_SWIPE 0.3
#define NOISEMASK 50.0
#define SEED 1
#define RND_MAX 32767
#define FSP 10.0
#define ALPHA 0.00275
#define BETA_1 9600.0
#define BETA_2 168.0
#define BETA_3 96000.0

/*  Command Name  */
char *cmnd;

typedef struct _float_list {
   float f;
   struct _float_list *next;
} float_list;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - pitch extraction\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -a a  : algorithm used for pitch        [%d]\n",
           ATYPE);
   fprintf(stderr, "               estimation\n");
   fprintf(stderr, "                 0 (RAPT)\n");
   fprintf(stderr, "                 1 (SWIPE')\n");
   fprintf(stderr, "       -s s  : sampling frequency (kHz)        [%.1f]\n",
           SAMPLE_FREQ);
   fprintf(stderr, "       -p p  : frame shift                     [%d]\n",
           FRAME_SHIFT);
   fprintf(stderr, "       -T T  : voiced/unvoiced threshold       [%.1f]\n",
           THRESH_RAPT);
   fprintf(stderr, "               (used only for RAPT algorithm)\n");
   fprintf(stderr, "       -t t  : voiced/unvoiced threshold       [%g]\n",
           THRESH_SWIPE);
   fprintf(stderr, "               (used only for SWIPE' algorithm)\n");
   fprintf(stderr, "       -L L  : minimum fundamental             [%g]\n",
           LOW);
   fprintf(stderr, "               frequency to search for (Hz)\n");
   fprintf(stderr, "       -H H  : maximum fundamental             [%g]\n",
           HIGH);
   fprintf(stderr, "               frequency to search for (Hz)\n");
   fprintf(stderr, "       -o o  : output format                   [%d]\n",
           OTYPE);
   fprintf(stderr, "                 0 (pitch)\n");
   fprintf(stderr, "                 1 (f0)\n");
   fprintf(stderr, "                 2 (log(f0))\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       waveform (%s)             \n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       pitch, f0 or log(f0) (%s)\n", FORMAT);
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
   int length, frame_shift = FRAME_SHIFT, atype = ATYPE, otype = OTYPE;
   double *x, thresh_rapt = THRESH_RAPT, thresh_swipe =
       THRESH_SWIPE, sample_freq = SAMPLE_FREQ, L = LOW, H = HIGH;
   FILE *fp = stdin;
   float_list *top, *cur, *prev;
   void rapt(float_list * flist, int length, double sample_freq,
             int frame_shift, double min, double max, double threshold,
             int otype);
   void swipe(float_list * input, int length, double sample_freq,
              int frame_shift, double min, double max, double threshold,
              int otype);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'a':
            atype = atoi(*++argv);
            --argc;
            break;
         case 's':
            sample_freq = atof(*++argv);
            --argc;
            break;
         case 'p':
            frame_shift = atoi(*++argv);
            --argc;
            break;
         case 'T':
            thresh_rapt = atof(*++argv);
            --argc;
            break;
         case 't':
            thresh_swipe = atof(*++argv);
            --argc;
            break;
         case 'L':
            L = atof(*++argv);
            --argc;
            break;
         case 'H':
            H = atof(*++argv);
            --argc;
            break;
         case 'o':
            otype = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else {
         fp = getfp(*argv, "rb");
      }
   sample_freq *= 1000.0;

   x = dgetmem(1);
   top = prev = (float_list *) malloc(sizeof(float_list));
   length = 0;
   prev->next = NULL;
   while (freadf(x, sizeof(*x), 1, fp) == 1) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = (float) x[0];
      length++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }

   if (atype == 0) {
      rapt(top->next, length, sample_freq, frame_shift, L, H, thresh_rapt,
           otype);
   } else {
      swipe(top->next, length, sample_freq, frame_shift, L, H, thresh_swipe,
            otype);
   }

   return (0);
}
