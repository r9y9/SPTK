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
*    Transform Real Sequence to Spectrum                                *
*                                                                       *
*                                  1985.11  K.Tokuda                    *
*                                  1996. 5  K.Koishida                  *
*                                  2010. 5  A.Tamamori                  *
*                                                                       *
*       usage:                                                          *
*               spec [options] [infile] > stdout                        *
*       options:                                                        *
*               -l l     :  frame length                      [256]     *
*               -m m     :  order of MA part                  [0]       *
*               -n n     :  order of AR part                  [0]       *
*               -z z     :  MA coefficients filename          [NULL]    *
*               -p p     :  AR coefficients filename          [NULL]    *
*               -e e     :  small value for calculating log() [0]       *
*               -E E     :  floor in db calculated per frame  [N/A]     *
*               -o o     :  output format                     [0]       *
*                             0 (20 * log|H(z)|)                        *
*                             1 (ln|H(z)|)                              *
*                             2 (|H(z)|)                                *
*                             3 (|H(z)|^2)                              *
*       infile:                                                         *
*               real sequence                                           *
*                       , c(0), c(1), ..., c(l-1),                      *
*       stdout:                                                         *
*               spectrum                                                *
*                       , s(0), s(1), ..., s(L/2),                      *
*       notice:                                                         *
*               value of e must be e>=0                                 *
*               value of E must be E<0                                  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: spec.c,v 1.30 2013/12/16 09:02:03 mataki Exp $";


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
#define LENG    256
#define ORDERMA 0
#define ORDERAR 0
#define ETYPE   0
#define EPS     0.0
#define OTYPE   0

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transform real sequence to spectrum\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : frame length                      [%d]\n",
           LENG);
   fprintf(stderr, "       -m m  : order of MA part                  [%d]\n",
           ORDERMA);
   fprintf(stderr, "       -n n  : order of AR part                  [%d]\n",
           ORDERAR);
   fprintf(stderr, "       -z z  : MA coefficients filename          [NULL]\n");
   fprintf(stderr, "       -p p  : AR coefficients filename          [NULL]\n");
   fprintf(stderr, "       -e e  : small value for calculating log() [%g]\n",
           EPS);
   fprintf(stderr, "       -E E  : floor in db calculated per frame  [N/A]\n");
   fprintf(stderr, "       -o o  : output format                     [%d]\n",
           OTYPE);
   fprintf(stderr, "                 0 (20 * log|H(z)|)\n");
   fprintf(stderr, "                 1 (ln|H(z)|)\n");
   fprintf(stderr, "                 2 (|H(z)|)\n");
   fprintf(stderr, "                 3 (|H(z)|^2)\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       real sequences (%s)                    [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       spectrum (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       value of e must be e>=0\n");
   fprintf(stderr, "       value of E must be E<0\n");
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
   int leng = LENG, otype = OTYPE, etype = ETYPE, orderma = ORDERMA, orderar =
       ORDERAR, no, i;
   char *filema = "", *filear = "";
   FILE *fp = stdin, *fpma = NULL, *fpar = NULL;
   double eps = EPS, eps2 = 1, k, *x, *y, *mag;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            leng = atoi(*++argv);
            --argc;
            break;
         case 'm':
            orderma = atoi(*++argv);
            --argc;
            break;
         case 'n':
            orderar = atoi(*++argv);
            --argc;
            break;
         case 'z':
            filema = *++argv;
            --argc;
            break;
         case 'p':
            filear = *++argv;
            --argc;
            break;
         case 'e':
            etype = 1;
            eps = atof(*++argv);
            --argc;
            break;
         case 'E':
            etype = 2;
            eps2 = atof(*++argv);
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
      } else
         fp = getfp(*argv, "rb");

   if (etype == 1 && eps < 0.0) {
      fprintf(stderr, "%s : value of e must be e >= 0!\n", cmnd);
      usage(1);
   }

   if (etype == 2 && eps2 >= 0.0) {
      fprintf(stderr, "%s : value of E must be E < 0!\n", cmnd);
      usage(1);
   }

   no = leng / 2 + 1;

   x = dgetmem(leng + leng + no);
   y = x + leng;
   mag = y + leng;

   if ((*filema == '\0') && (*filear == '\0')) {
      if (orderar > 0)
         fpar = fp;
      else
         fpma = fp;

      if (orderma <= 0)
         orderma = leng - 1;
   }

   if (*filema != '\0')
      fpma = getfp(filema, "rb");
   if (*filear != '\0')
      fpar = getfp(filear, "rb");

   for (;;) {
      if ((fpma == NULL) && (fpar != NULL))
         for (i = 0; i < no; i++)
            mag[i] = 1.0;
      else {
         fillz(x, sizeof(*x), leng);
         if (freadf(x, sizeof(*x), orderma + 1, fpma) == 0)
            return (0);

         fftr(x, y, leng);
         for (i = 0; i < no; i++)
            mag[i] = x[i] * x[i] + y[i] * y[i];
      }

      if (fpar != NULL) {
         fillz(x, sizeof(*x), leng);
         if (freadf(x, sizeof(*x), orderar + 1, fpar) == 0)
            return (0);
         k = x[0] * x[0];
         x[0] = 1.0;
         fftr(x, y, leng);
         for (i = 0; i < no; i++)
            mag[i] *= k / (x[i] * x[i] + y[i] * y[i]);
      }

      if (otype == 0 || otype == 1) {
         double max, min;
         if (etype == 1 && eps >= 0.0) {
            for (i = 0; i < no; i++) {
               mag[i] = mag[i] + eps;
            }
         } else if (etype == 2 && eps2 < 0) {
            max = mag[0];
            for (i = 1; i < no; i++) {
               if (max < mag[i])
                  max = mag[i];
            }
            max = sqrt(max);
            min = max * pow(10.0, eps2 / 20.0); /* floor is 20*log10(min/max) */
            min = min * min;
            for (i = 0; i < no; i++) {
               if (mag[i] < min)
                  mag[i] = min;
            }
         }
      }

      switch (otype) {
      case 1:
         for (i = 0; i < no; i++)
            x[i] = 0.5 * log(mag[i]);
         fwritef(x, sizeof(*x), no, stdout);
         break;
      case 2:
         for (i = 0; i < no; i++)
            x[i] = sqrt(mag[i]);
         fwritef(x, sizeof(*x), no, stdout);
         break;
      case 3:
         for (i = 0; i < no; i++)
            x[i] = mag[i];
         fwritef(x, sizeof(*x), no, stdout);
         break;
      default:
         for (i = 0; i < no; i++)
            x[i] = 10 * log10(mag[i]);
         fwritef(x, sizeof(*x), no, stdout);
         break;
      }
   }

   return (0);
}
