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
*    Transform  Cepstrum to Spectrum                                    *
*                                                                       *
*                                       1986. 9  K.Tokuda               *
*                                       1996. 4  K.Koishida             *
*                                                                       *
*       usage:                                                          *
*               c2sp [ options ] [ infile ] > stdout                    *
*       options:                                                        *
*               -m m     :  order of cepstrum           [25]            *
*               -l l     :  frame length                [256]           *
*               -p       :  output phase                [FALSE]         *
*               -o o     :  output format               [0]             *
*                            0 (20 * log|H(z)|)                         *
*                            1 (ln|H(z)|)                               *
*                            2 (|H(z)|)                                 *
*                           -p option is specified                      *
*                            0 (arg|H(z)| / pi          [pi rad])       *
*                            1 (arg|H(z)|               [rad])          *
*                            2 (arg|H(z)| * 180 / pi    [deg])          *
*       infile:                                                         *
*               cepstrum                                                *
*                   , c(0), c(1), ..., c(M),                            *
*       stdout:                                                         *      
*               spectrum                                                *
*                   , s(0), s(1), ..., s(L/2),                          *
*       require:                                                        *
*               c2sp()                                                  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: c2sp.c,v 1.24 2013/12/16 09:01:54 mataki Exp $";


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
#define LENG 256
#define PHASE FA
#define OTYPE 0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transform cepstrum to spectrum\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of cepstrum      [%d]\n", ORDER);
   fprintf(stderr, "       -l l  : frame length           [%d]\n", LENG);
   fprintf(stderr, "       -p    : output phase           [%s]\n", BOOL[PHASE]);
   fprintf(stderr, "       -o o  : output format          [%d]\n", OTYPE);
   fprintf(stderr, "                0 (20*log|H(z)|)\n");
   fprintf(stderr, "                1 (ln|H(z)|)\n");
   fprintf(stderr, "                2 (|H(z)|)\n");
   fprintf(stderr, "               -p option is specified\n");
   fprintf(stderr, "                0 (arg|H(z)|/pi       [pi rad])\n");
   fprintf(stderr, "                1 (arg|H(z)|          [rad])\n");
   fprintf(stderr, "                2 (arg|H(z)|*180 / pi [deg])\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       cepstrum (%s)             [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       spectrum (%s)\n", FORMAT);
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
   int leng = LENG, m = ORDER, start = 0, end = 0, otype = OTYPE, i, no;
   FILE *fp = stdin;
   char phase = PHASE;
   double logk, *x, *y, *c;

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
         case 'l':
            leng = atoi(*++argv);
            --argc;
            break;
         case 'p':
            phase = 1 - phase;
            break;
         case 'o':
            otype = atoi(*++argv);
            --argc;
            break;
         case 's':
            start = atoi(*++argv);
            --argc;
            break;
         case 'e':
            end = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   if (end == 0)
      end = m;

   x = dgetmem(leng * 2);
   y = x + leng;
   c = dgetmem(m + 1);

   no = leng / 2 + 1;
   logk = 20.0 / log(10.0);

   while (freadf(c, sizeof(*c), m + 1, fp) == m + 1) {
      fillz(c, sizeof(*c), start);
      for (i = end + 1; i <= m; i++)
         c[i] = 0.0;

      c2sp(c, m, x, y, leng);

      if (phase)
         switch (otype) {
         case 1:
            for (i = no; i--;)
               x[i] = y[i];
            break;
         case 2:
            for (i = no; i--;)
               x[i] = y[i] * 180 / PI;
            break;
         default:
            for (i = no; i--;)
               x[i] = y[i] / PI;
            break;
      } else
         switch (otype) {
         case 1:
            break;
         case 2:
            for (i = no; i--;)
               x[i] = exp(x[i]);
            break;
         default:
            for (i = no; i--;)
               x[i] *= logk;
            break;
         }

      fwritef(x, sizeof(*x), no, stdout);
   }

   return (0);
}
