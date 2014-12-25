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

/*************************************************************************
*                                                                        *
*    MFCC Analysis                                                       *
*                                                                        *
*                                       2011.9 T.Sawada                  *
*                                                                        *
*       usage:                                                           *
*               mfcc [ options ] [ infile ] > stdout                     *
*       options:                                                         *
*               -a  a    :  pre-emphasis coefficient             [0.97]  *
*               -c  c    :  liftering coefficient                [0]     *
*               -e  e    :  flooring value for culculating log(x)[1.0]   *
*                           in filterbank analysis                       *
*                           if x < e then x = e                          *
*               -l  l    :  frame length of input                [256]   *
*               -L  L    :  frame length of window               [256]   * 
*               -m  m    :  order of cepstrum                    [13]    *
*               -n  n    :  order of channel for mel-filter bank [26]    *
*               -s  s    :  sampling frequency (kHz)             [16.0]  *
*               -d       :  without using fft algorithm          [FALSE] *
*               -w       :  use hamming window                   [FALSE] *
*               -E       :  use power                            [FALSE] *
*               -0       :  use 0'th static coefficient          [FALSE] *
*       infile:                                                          *
*               data sequence                                            *
*                   , x(0), x(1), ..., x(l-1),                           *
*       stdout:                                                          *
*               mel-frequency cepstral coefficients                      *
*                   , mc(0), mc(1), ..., mc(m-1),                        *
*               if -E or -0 option is given, Energy E and 0'th static    *
*               coefficient C0 is output as follows,                     *
*                   , mc(0), mc(1), ..., mc(m-1), E (C0)                 *
*               if Both -E and -0 option is given, C0 is output before E *
*                                                                        *
*       require:                                                         *
*               mfcc()                                                   *
*                                                                        *
*************************************************************************/

static char *rcs_id = "$Id: mfcc.c,v 1.11 2014/12/11 08:30:41 uratec Exp $";


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

/*  Default Values  */
#define ORDER 12
#define WLNG 256
#define EPS 1.0
#define CHANNEL 20
#define USEHAMMING FA
#define DFTMODE FA
#define CZERO FA
#define ENERGY  FA
#define SAMPLEFREQ 16.0
#define ALPHA 0.97
#define LIFT 22
#define WTYPE 0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - MFCC analysis\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -a a  : pre-emphasis coefficient              [%g]\n",
           ALPHA);
   fprintf(stderr,
           "       -c c  : liftering coefficient                 [%d]\n", LIFT);
   fprintf(stderr,
           "       -e e  : flooring value for calculating log(x) [%g]\n", EPS);
   fprintf(stderr, "               in filterbank analysis\n");
   fprintf(stderr, "               if x < e, then x = e\n");
   fprintf(stderr,
           "       -s s  : sampling frequency (kHz)              [%.1f]\n",
           SAMPLEFREQ);
   fprintf(stderr,
           "       -l l  : frame length of input                 [%d]\n", WLNG);
   fprintf(stderr,
           "       -L L  : frame length for fft                  [2^n]\n");
   fprintf(stderr, "               default value 2^n satisfies l < 2^n\n");
   fprintf(stderr,
           "       -m m  : order of cepstrum                     [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -n n  : order of channel for mel-filter bank  [%d]\n",
           CHANNEL);
   fprintf(stderr,
           "       -w w  : type of window                        [%d]\n",
           WTYPE);
   fprintf(stderr, "                  0 (hamming)\n");
   fprintf(stderr, "                  1 (do not use a window function)\n");
   fprintf(stderr,
           "       -d    : without using fft algorithm (use dft) [%s]\n",
           BOOL[DFTMODE]);
   fprintf(stderr,
           "       -E    : output energy                         [%s]\n",
           BOOL[ENERGY]);
   fprintf(stderr,
           "       -0    : output 0'th static coefficient        [%s]\n",
           BOOL[CZERO]);
   fprintf(stderr, "\n");
   fprintf(stderr, "       if -E or -0 option is given, the value is output\n");
   fprintf(stderr,
           "       after the MFCC. Also, if both -E and -0 option are\n");
   fprintf(stderr,
           "       given, 0'th static coefficient C0 is output before energy E.\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       sequence (%s)   [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       mfcc (%s)\n", FORMAT);
   fprintf(stderr, "  note:\n");
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
   int m = ORDER, l = WLNG, L = -1, n = CHANNEL, lift = LIFT, wtype =
       WTYPE, num = 0;
   double eps = EPS, fs = SAMPLEFREQ, alpha = ALPHA, *x, *mc;
   FILE *fp = stdin;
   Boolean dftmode = DFTMODE, czero = CZERO, usehamming = USEHAMMING;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'a':
            alpha = atof(*++argv);
            --argc;
            break;
         case 'c':
            lift = atoi(*++argv);
            --argc;
            break;
         case 'e':
            eps = atof(*++argv);
            --argc;
            break;
         case 'f':
            fs = atof(*++argv);
            --argc;
            break;
         case 's':
            fs = atof(*++argv);
            --argc;
            break;
         case 'l':
            l = atoi(*++argv);
            --argc;
            break;
         case 'L':
            L = atoi(*++argv);
            --argc;
            break;
         case 'm':
            m = atoi(*++argv);
            --argc;
            break;
         case 'n':
            n = atoi(*++argv);
            --argc;
            break;
         case 'w':
            wtype = atoi(*++argv);
            --argc;
            break;
         case 'd':
            dftmode = 1 - dftmode;
            break;
         case 'E':
            num++;
            break;
         case '0':
            czero = 1 - czero;
            num++;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   fs *= 1000;                  /* kHz -> Hz */

   if (L < 0)
      for (L = 2; L <= l; L *= 2) {
      }
   if (wtype == 0)
      usehamming = 1 - usehamming;

   x = dgetmem(l + m + 2);
   mc = x + l;

   while (freadf(x, sizeof(*x), l, fp) == l) {

      mfcc(x, mc, fs, alpha, eps, l, L, m + 1, n, lift, dftmode, usehamming);
      if (!czero)
         mc[m] = mc[m + 1];
      fwritef(mc, sizeof(*mc), m + num, stdout);
   }

   return 0;
}
