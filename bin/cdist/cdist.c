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
*    Calculation of Cepstral Distance                                   *
*                                       1996.7  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               cdist [ options ] cfile [ infile ] > stdout             *
*       options:                                                        *
*               -m m     :  order of minimum-phase cepstrum   [25]      *
*               -o o     :  output format                     [0]       *
*                               0 ([dB])                                *
*                               1 (squared error)                       *
*                               2 (root squared error)                  *
*               -f       :  frame length                      [FALSE]   *
*       cfile:                                                          *
*       infile:                                                         *
*               minimum-phase cepstrum                                  *
*                          , c(0), c(1), ..., c(m),                     *
*       stdout:                                                         *
*               cepstral distance                                       *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: cdist.c,v 1.25 2014/12/11 08:30:32 uratec Exp $";


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

#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define ORDER 25
#define FRAME FA
#define OTYPE 0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - calculation of cepstral distance\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] cfile [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "      -m m  : order of minimum-phase cepstrum [%d]\n",
           ORDER);
   fprintf(stderr, "      -o o  : output format                   [%d]\n",
           OTYPE);
   fprintf(stderr, "                0 ([dB])\n");
   fprintf(stderr, "                1 (squared error)\n");
   fprintf(stderr, "                2 (root squared error)\n");
   fprintf(stderr, "      -f    : output frame by frame           [%s]\n",
           BOOL[FRAME]);
   fprintf(stderr, "      -h    : print this message\n");
   fprintf(stderr, "  cfile:\n");
   fprintf(stderr, "  infile:                                     [stdin]\n");
   fprintf(stderr, "      minimum-phase cepstrum (%s)\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "      cepstral distance (%s)\n", FORMAT);
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
   int m = ORDER, num = 0, otype = OTYPE, i;
   FILE *fp = stdin, *fp1 = NULL;
   double *x, *y, sub, sum, z = 0.0;
   Boolean frame = FRAME;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':
            m = atoi(*++argv);
            --argc;
            break;
         case 'o':
            otype = atoi(*++argv);
            --argc;
            break;
         case 'f':
            frame = 1 - frame;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (fp1 == NULL)
         fp1 = getfp(*argv, "rb");
      else
         fp = getfp(*argv, "rb");

   x = dgetmem(m + m + 2);
   y = x + m + 1;

   while (freadf(x, sizeof(*x), m + 1, fp) == m + 1
          && freadf(y, sizeof(*y), m + 1, fp1) == m + 1) {
      sum = 0.0;
      for (i = 1; i <= m; i++) {
         sub = x[i] - y[i];
         sum += sub * sub;
      }

      if (otype == 0) {
         sum = sqrt(2.0 * sum);
         sum *= LN_TO_LOG;
      } else if (otype == 2)
         sum = sqrt(sum);

      if (!frame) {
         z += sum;
         num++;
      } else
         fwritef(&sum, sizeof(sum), 1, stdout);
   }

   if (!frame) {
      z = z / (double) num;
      fwritef(&z, sizeof(z), 1, stdout);
   }

   return 0;
}
