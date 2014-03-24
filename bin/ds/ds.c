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
*       Sampling Rate Conversion (Down Sampling)                        *
*                                                                       *
*                                       1994. 5  T.Kobayashi            *
*                                       1997. 2  G.Hirabayashi          *
*                                       1998. 6  M.Tamura               *
*                                                                       *
*       usage:                                                          *
*               ds [ options ] [ infile ] > stdout                      *
*       options:                                                        *
*               -s s    :  conversion type              [21]            *
*                           21   down sampling by 2:1                   *
*                           32   down sampling by 3:2                   *
*                           43   down sampling by 4:3                   *
*                           52   down sampling by 5:2                   *
*                           54   down sampling by 5:4                   *
*                           74   down sampling by 7:4                   *
*       infile:                                                         *
*               data sequence (float)                   [stdin]         *
*       stdout:                                                         *
*               converted data sequence (float)                         *
*       notice:                                                         *
*               Default LPF coefficients File                           *
*                2:1 -> /usr/local/share/SPTK/lpfcoef.2to1              *
*                3:2 -> /usr/local/share/SPTK/lpfcoef.3to2              *
*                4:3 -> /usr/local/share/SPTK/lpfcoef.4to3              *
*                5:2 -> /usr/local/share/SPTK/lpfcoef.5to2up            *
*                5:2 -> /usr/local/share/SPTK/lpfcoef.5to2dn            *
*                5:4 -> /usr/local/share/SPTK/lpfcoef.5to2up            *
*                5:4 -> /usr/local/share/SPTK/lpfcoef.5to2dn            *
*                7:4 -> /usr/local/share/SPTK/lpfcoef.7to4              *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: ds.c,v 1.25 2013/12/22 16:58:55 uratec Exp $";

/* Standard C Libraries  */
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

/* Default Values */
#define DECRATE7_4 7
#define INTRATE7_4 4
#define DECRATE5 5
#define INTRATE5 2
#define DECRATE4_3 4
#define INTRATE4_3 3
#define DECRATE3_2 3
#define INTRATE3_2 2
#define DECRATE2_1 2
#define INTRATE2_1 1

#define RBSIZE 512
#define SIZE 256
#define STYPE 21

#ifndef LIB
#define LIB "/usr/local/share/SPTK"
#endif

#define COEF2_1 LIB "/lpfcoef.2to1"
#define COEF3_2 LIB "/lpfcoef.3to2"
#define COEF4_3 LIB "/lpfcoef.4to3"
#define COEF5UP LIB "/lpfcoef.5to2up"
#define COEF5DN LIB "/lpfcoef.5to2dn"
#define COEF7_4 LIB "/lpfcoef.7to4"

#define mod(x) ((x) & (RBSIZE -1))

static char *cmnd, *coef, *coef1;
static int start = -1, intrate, decrate, type = STYPE;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - sampling rate conversion (down sampling)\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -s s  : conversion type              [%d]\n", STYPE);
   fprintf(stderr, "                21  down sampling by 2:1\n");
   fprintf(stderr, "                32  down sampling by 3:2\n");
   fprintf(stderr, "                43  down sampling by 4:3\n");
   fprintf(stderr, "                52  down sampling by 5:2\n");
   fprintf(stderr, "                54  down sampling by 5:4\n");
   fprintf(stderr, "                74  down sampling by 7:4\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)                [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       converted data sequence (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       Default LPF coefficients File\n");
   fprintf(stderr, "        2:1 -> %s\n", COEF2_1);
   fprintf(stderr, "        3:2 -> %s\n", COEF3_2);
   fprintf(stderr, "        4:3 -> %s\n", COEF4_3);
   fprintf(stderr, "        5:2 -> %s\n", COEF5UP);
   fprintf(stderr, "        5:2 -> %s\n", COEF5DN);
   fprintf(stderr, "        5:4 -> %s\n", COEF5UP);
   fprintf(stderr, "        5:4 -> %s\n", COEF5DN);
   fprintf(stderr, "        7:4 -> %s\n", COEF7_4);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

int main(int argc, char *argv[])
{
   FILE *fp = stdin;
   void decimate(FILE * fp);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc)
      if (*(*++argv) == '-') {
         switch (*(*argv + 1)) {
         case 's':
            type = atoi(*++argv);
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

   switch (type) {
   case 21:
      coef = COEF2_1;
      decrate = DECRATE2_1;
      intrate = INTRATE2_1;
      break;
   case 32:
      coef = COEF3_2;
      decrate = DECRATE3_2;
      intrate = INTRATE3_2;
      break;
   case 43:
      coef = COEF4_3;
      decrate = DECRATE4_3;
      intrate = INTRATE4_3;
      break;
   case 52:
   case 54:
      coef = COEF5DN;
      coef1 = COEF5UP;
      decrate = DECRATE5;
      intrate = INTRATE5;
      break;
   case 74:
      coef = COEF7_4;
      decrate = DECRATE7_4;
      intrate = INTRATE7_4;
      break;
   default:
      fprintf(stderr, "%s : Given dec/int rate %d is not supported!\n", cmnd,
              type);
      return (1);
   }
   decimate(fp);
   fclose(fp);

   return 0;
}

void decimate(FILE * fp)
{
   int is_cont, i, k, nread, count, nwr, delay;
   double x[SIZE], y[SIZE];

   void firin(double in);
   void firin0(void);
   double firout(int os);
   void firinit(void);

   firinit();
   delay = start;
   for (count = is_cont = 1; is_cont;) {
      if ((nread = freadf(x, sizeof(*x), SIZE, fp)) == 0) {
         is_cont = 0;
         nread = (decrate * start) / intrate;
         if (type == 54)
            nread /= 2;
         for (k = 0; k < nread; k++)
            x[k] = 0;
      }
      for (nwr = k = 0; k < nread; k++) {
         firin(x[k]);
         for (i = 0; i < intrate; i++) {
            if (--count == 0) {
               y[nwr++] = firout(i);
               count = decrate;
            }
         }
         if (type == 54) {
            firin0();
            for (i = 0; i < intrate; i++) {
               if (--count == 0) {
                  y[nwr++] = firout(i);
                  count = decrate;
               }
            }
         }
      }
      if (delay) {
         if (nwr > delay) {
            nwr -= delay;
            if (fwritef(y + delay, sizeof(*y), nwr, stdout) != nwr) {
               fprintf(stderr, "%s : File write error!\n", cmnd);
               exit(1);
            }
            delay = 0;
         } else {
            delay -= nwr;
         }
      } else {
         if (fwritef(y, sizeof(*y), nwr, stdout) != nwr) {
            fprintf(stderr, "%s : File write error!\n", cmnd);
            exit(1);
         }
      }
   }
}

static double rb[RBSIZE], rb2[RBSIZE], hdn[RBSIZE + 1], hup[RBSIZE + 1];
static int flengdn = -1, flengup = -1, indx = 0, indx2 = 0;

void firin(double in)
{
   double out;
   int k, l;

   indx = mod(indx - 1);

   switch (type) {
   case 52:
      rb2[indx] = in;
      out = 0;
      for (k = 0, l = indx; k <= flengup; k++, l = mod(l + 1))
         out += rb2[l] * hup[k];
      rb[indx] = out;
      break;
   case 54:
      indx2 = mod(indx2 - 1);
      rb2[indx2] = 2 * in;
      out = 0;
      for (k = 0, l = indx2; k <= flengup; k += intrate, l = mod(l + 1))
         out += rb2[l] * hup[k];
      rb[indx] = out;
      break;
   default:
      rb[indx] = in;
   }
}

void firin0(void)
{
   double out;
   int k, l;

   out = 0;
   for (k = 1, l = indx2; k <= flengup; k += intrate, l = mod(l + 1))
      out += rb2[l] * hup[k];

   indx = mod(indx - 1);
   rb[indx] = out;
}

double firout(int os)
{
   double out;
   int k, l;

   out = 0;
   for (k = os, l = indx; k <= flengdn; k += intrate, l = mod(l + 1))
      out += rb[l] * hdn[k];

   return (out);
}

void firinit(void)
{
   FILE *fp;

   fp = getfp(coef, "rt");
   flengdn = freada(hdn, RBSIZE + 1, fp);
   fclose(fp);
   if (--flengdn < 0) {
      fprintf(stderr, "%s : Cannot read filter coefficients!\n", cmnd);
      exit(1);
   }
   if (type == 52 || type == 54) {
      fp = getfp(coef1, "rt");
      flengup = freada(hup, RBSIZE + 1, fp);
      fclose(fp);
      if (--flengup < 0) {
         fprintf(stderr, "%s : Cannot read filter coefficients!\n", cmnd);
         exit(1);
      }
      start = (((flengup / 2) * intrate) + (flengdn / 2)) / decrate;
   } else
      start = flengdn / (2 * decrate);
}
