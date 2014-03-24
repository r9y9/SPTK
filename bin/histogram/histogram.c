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
*    Histogram                                                          *
*                                                                       *
*                                       1986.9   K.Tokuda               *
*                                       1995.7   K.Tokuda               *
*                                       1996.3   N.Miyazaki             *
*                                       2000.3   Y.Nankaku              *
*                                                                       *
*       usage:                                                          *
*               histogram [ options ] [ infile ] > stdout               *
*       options:                                                        *
*               -l l    : frame size                    [0]             *
*               -i i    : infimum                       [0.0]           *
*               -j j    : supremum                      [1.0]           *
*               -s s    : step size                     [0.1]           *
*               -n      : normalization                 [FALSE]         *
*       infile:                                                         *
*               sample data (float)                                     * 
*       stdout:                                                         *
*               histogram (float)                                       *
*                                                                       *
*       notice:                                                         *
*               if l>0,calculate histogram frame by frame               *
************************************************************************/

static char *rcs_id =
    "$Id: histogram.c,v 1.25 2013/12/16 09:01:57 mataki Exp $";

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

/*  Command Name  */
char *cmnd;

int usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - histogram\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : frame size         [0]\n");
   fprintf(stderr, "       -i i  : infimum            [0.0]\n");
   fprintf(stderr, "       -j j  : supremum           [1.0]\n");
   fprintf(stderr, "       -s s  : step size          [0.1]\n");
   fprintf(stderr, "       -n    : normalization      [FALSE]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)      [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       histogram (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       if l > 0, calculate histogram frame by frame\n");
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
   char *s, c;
   double i = 0, j = 1, st = 0.1;
   int l = 0, k, ii;
   int flg = 0, n = 0;
   double *h;
   double *x, xx;
   FILE *fp = stdin;


   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if ((c != 'n') && (*++s == '\0')) {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'l':
            l = atoi(s);
            break;
         case 'i':
            i = atof(s);
            break;
         case 'j':
            j = atof(s);
            break;
         case 's':
            st = atof(s);
            break;
         case 'n':
            n = 1;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, c);
            usage(1);
            break;
         }
      } else
         fp = getfp(*argv, "rb");
   }

   k = (int) ((j - i) / st + 1.0);
   h = dgetmem(k + 2);

   if (l) {
      x = dgetmem(l);
      while (freadf(x, sizeof(*x), l, fp) == l) {
         flg += histogram(x, l, i, j, st, h);

         if (n && l)
            for (ii = 0; ii <= k; ii++)
               h[ii] /= (double) l;

         fwritef(h, sizeof(*h), k, stdout);
      }
   } else {
      fillz(h, sizeof(*h), k + 2);
      while (freadf(&xx, sizeof(xx), 1, fp) == 1) {
         if ((xx < i) || (xx > j)) {
            flg = 1;
         } else {
            for (ii = 0; ii <= k; ii++) {
               if (xx < i + (ii + 1) * st) {
                  h[ii]++;
                  break;
               }
               l++;
            }
         }
      }

      if (n && l)
         for (ii = 0; ii <= k; ii++)
            h[ii] /= (double) l;

      fwritef(h, sizeof(*h), k, stdout);
   }

   return flg;
}
