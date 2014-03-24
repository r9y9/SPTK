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
*    Check stability of MLSA filter coefficients                        *
*                                                                       *
*                                         2012.9  Akira Tamamori        *
*                                                                       *
*       usage:                                                          *
*               mlsacheck [ options ] [ infile] > stdout                *
*       options:                                                        *
*               -m m     :  order of mel-cepstrum               [25]    *
*               -a a     :  all-pass constant                   [0.35]  *
*               -l L     :  FFT length                          [256]   *
*               -c C     :  modify MLSA filter coefficients     [0]     *
*                           of unstable frames                          *
*                             0 : no modification                       *
*                             1 : clipping                              *
*                             2 : scaling                               *
*               -r r     :  stable condition for MLSA filter    [0]     *
*                             0 : keeping log approximation error       *
*                                 not exceeding 0.24 dB (P=4)           *
*                                 or 0.2735 dB (P=5)                    *
*                             1 : keeing MLSA filter stable             *
*               -P P     :  order of Pade approximation         [4]     *
*               -R R     :  threshold value for modification    [N/A]   *
*                           if this option wasn't specified,            *
*                             r=0,P=4 : R=4.5                           *
*                             r=1,P=4 : R=6.2                           *
*                             r=0,P=5 : R=6.0                           *
*                             r=1,P=5 : R=7.65                          *
*       infile:                                                         *
*               mel cepstral coefficients                               *
*       stdout:                                                         *
*               mel-cepstrums satisfying stability condition            *
*       notice:                                                         *
*               P = 4 or 5                                              *
*                                                                       *
************************************************************************/

static char *rcs_id =
    "$Id: mlsacheck.c,v 1.15 2013/12/23 00:02:41 mataki Exp $";


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

#include <ctype.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define ORDER         25
#define ALPHA         0.35
#define PADEORDER     4
#define FFTLENGTH     256
#define PADE4_THRESH1 4.5
#define PADE4_THRESH2 6.2
#define PADE5_THRESH1 6.0
#define PADE5_THRESH2 7.65
#define STABLE1       0
#define STABLE2       1

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr,
           " %s - check stability condition of MLSA filter coefficients \n",
           cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m  : order of mel-cepstrum                 [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -a a  : all-pass constant                     [%g]\n",
           ALPHA);
   fprintf(stderr,
           "       -l L  : FFT length                            [%d]\n",
           FFTLENGTH);
   fprintf(stderr,
           "       -c C  : modify MLSA filter coefficients of    [0]\n");
   fprintf(stderr, "               unstable frames \n");
   fprintf(stderr, "                 0 : no modification\n");
   fprintf(stderr, "                 1 : clipping\n");
   fprintf(stderr, "                 2 : scaling\n");
   fprintf(stderr,
           "       -r r  : stability condion for MLSA filter     [%d]\n",
           STABLE1);
   fprintf(stderr, "                 0 : keeping log approximation error\n");
   fprintf(stderr, "                     not exceeding 0.24 dB (P=4)\n");
   fprintf(stderr, "                     or 0.2735 dB (P=5) \n");
   fprintf(stderr, "                 1 : keeping MLSA filter stable\n");
   fprintf(stderr,
           "       -P P  : order of Pade approximation           [%d]\n",
           PADEORDER);
   fprintf(stderr,
           "       -R R  : threshold value for modification      [N/A]\n");
   fprintf(stderr, "               if this option wasn't specified, \n");
   fprintf(stderr, "                 r=0,P=4 : R=4.5\n");
   fprintf(stderr, "                 r=1,P=4 : R=6.2\n");
   fprintf(stderr, "                 r=0,P=5 : R=6.0\n");
   fprintf(stderr, "                 r=1,P=5 : R=7.65\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       mel-cepstrums (%s)                         [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       mel-cepstrums satisfying stability condition (%s)\n",
           FORMAT);
   fprintf(stderr, "  stderr:\n");
   fprintf(stderr, "       ascii report of unstable frame\n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       P = 4 or 5 \n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

void mlsacheck(double *mcep, int m, int fftlen, int frame,
               double a, double r1, double r2, int c, int stable_condition)
{
   int i;
   double gain, r, *x, *y, mag, max = 0.0, tmp = 1.0;
   Boolean ascii_report = FA;

   x = dgetmem(fftlen);
   y = dgetmem(fftlen);

   fillz(x, sizeof(*x), fftlen);
   fillz(y, sizeof(*y), fftlen);

   /* calculate gain factor */
   for (i = 0, gain = 0.0; i < m + 1; i++) {
      x[i] = mcep[i];
      gain += x[i] * pow(-a, i);
   }

   /* gain normalization */
   x[0] -= gain;

   fftr(x, y, fftlen);

   /* check stability */
   for (i = 0; i < fftlen / 2 + 1; i++) {
      mag = x[i] * x[i] + y[i] * y[i];
      mag = sqrt(mag);

      switch (stable_condition) {
      case STABLE1:
         if (mag > r1) {
            ascii_report = TR;
            if (c == 2) {
               if (mag > max)
                  max = mag;
            }
            if (c == 1) {
               r = r1 / mag;
               x[i] *= r;
               y[i] *= r;
               if (i != 0 && i != fftlen / 2) {
                  x[fftlen - i] *= r;
                  y[fftlen - i] *= r;
               }
            }
         }
         break;
      case STABLE2:
         if (mag > r2) {
            ascii_report = TR;
            if (c == 2) {
               if (mag > max)
                  max = mag;
            }
            if (c == 1) {
               r = r2 / mag;
               x[i] *= r;
               y[i] *= r;
               if (i != 0 && i != fftlen / 2) {
                  x[fftlen - i] *= r;
                  y[fftlen - i] *= r;
               }
            }
         }
      }
   }

   if (c == 2) {
      switch (stable_condition) {
      case STABLE1:
         tmp = r1 / max;
         break;
      case STABLE2:
         tmp = r2 / max;
         break;
      }
      if (tmp < 1.0) {
         for (i = 0; i < fftlen; i++) {
            switch (stable_condition) {
            case STABLE1:
               x[i] *= tmp;
               y[i] *= tmp;
               break;
            case STABLE2:
               x[i] *= tmp;
               y[i] *= tmp;
               break;
            }
         }
      }
   }

   ifft(x, y, fftlen);

   x[0] += gain;

   fwritef(x, sizeof(*x), m + 1, stdout);

   free(x);
   free(y);

   /* output ascii report */
   if (ascii_report == TR) {
      fprintf(stderr, "[No. %d] is unstable frame\n", frame);
   }
}

int main(int argc, char **argv)
{
   int m = ORDER, pd = PADEORDER, fftlen = FFTLENGTH, stable_condition =
       STABLE1, frame = 0, c = 0;
   double *mcep, a = ALPHA, r1 = PADE4_THRESH1, r2 = PADE4_THRESH2, R = 0.0;
   FILE *fp = stdin;

   if ((cmnd = strrchr(argv[0], '/')) == NULL) {
      cmnd = argv[0];
   } else {
      cmnd++;
   }
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':
            m = atoi(*++argv);
            --argc;
            break;
         case 'a':
            a = atof(*++argv);
            --argc;
            break;
         case 'P':
            pd = atoi(*++argv);
            --argc;
            break;
         case 'l':
            fftlen = atoi(*++argv);
            --argc;
            break;
         case 'c':
            c = atoi(*++argv);
            if ((c != 0 && c != 1 && c != 2) || isdigit(**argv) == 0) {
               fprintf(stderr,
                       "%s : '-c' option must be specified with 0, 1 or 2.\n",
                       cmnd);
               usage(1);
            }
            --argc;
            break;
         case 'r':
            stable_condition = atoi(*++argv);
            if (stable_condition != STABLE1 && stable_condition != STABLE2) {
               fprintf(stderr,
                       "%s : '-r' option must be specified with %d or %d.\n",
                       cmnd, STABLE1, STABLE2);
               usage(1);
            }
            --argc;
            break;
         case 'R':
            R = atof(*++argv);
            if (isdigit(**argv) == 0) {
               fprintf(stderr,
                       "%s : '-R' option must be specified by real number !\n",
                       cmnd);
               usage(1);
            }
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
   }

   switch (pd) {
   case 4:
      r1 = PADE4_THRESH1;
      r2 = PADE4_THRESH2;
      break;
   case 5:
      r1 = PADE5_THRESH1;
      r2 = PADE5_THRESH2;
      break;
   default:
      fprintf(stderr, "%s : Order of Pade approximation should be 4 or 5!\n",
              cmnd);
      usage(1);
   }
   if (R != 0.0) {
      r1 = R;
      r2 = R;
   }

   mcep = dgetmem(m + 1);

   /* check stability of MLSA filter and output */
   while (freadf(mcep, sizeof(*mcep), m + 1, fp) == m + 1) {
      mlsacheck(mcep, m, fftlen, frame, a, r1, r2, c, stable_condition);
      frame++;
   }

   return (0);
}
