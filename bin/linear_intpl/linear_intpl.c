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
*    Linear Interpolation of Data                                       *
*                                                                       *
*                                             1987.1  K.Tokuda          *
*                                             1996.4  K.Koishida        *
*                                                                       *
*       usage:                                                          *
*               linear-intpl [ options ] [ infile ] > stdout            *
*       options:                                                        *
*               -l l         : output length               [256]        *
*               -m m         : number of interpolation     [255]        *
*               -x xmin xmax : minimum of x & maximum of x [0.0 0.5]    *
*               -i xmin      : minimum of x                [0]          *
*               -j xmax      : maximum of x                [0.5]        *
*       infile:                                                         *
*               data sequence                                           *
*                       , x0, y0, x1, y1..., xK, yK                     *
*                       (x0 = xmin, xk = xmax)                          *
*       stdout:                                                         *
*               interpolated data                                       *
*                       , y0, ..., yn                                   *
*                                                                       *
************************************************************************/

static char *rcs_id =
    "$Id: linear_intpl.c,v 1.25 2013/12/16 09:01:58 mataki Exp $";


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
#define LENG 255
#define MIN 0.0
#define MAX 0.5

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - linear interpolation of data\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l         : output length               [%d]\n",
           LENG + 1);
   fprintf(stderr, "       -m m         : number of interpolation     [%d]\n",
           LENG);
   fprintf(stderr,
           "       -x xmin xmax : minimum of x & maximum of x [%g %g]\n", MIN,
           MAX);
   fprintf(stderr, "       -i xmin      : minimum of x                [%g]\n",
           MIN);
   fprintf(stderr, "       -j xmax      : maximum of x                [%g]\n",
           MAX);
   fprintf(stderr, "       -h           : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)                      [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       interpolated data sequence (%s)\n", FORMAT);
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
   int l = LENG, ii, end;
   FILE *fp = stdin;
   double i = MIN, j = MAX, x1, y1, x2, y2, y, x, t;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            l = atoi(*++argv) - 1;
            --argc;
            break;
         case 'm':
            l = atoi(*++argv);
            --argc;
            break;
         case 'x':
            i = atof(*++argv);
            --argc;
            j = atof(*++argv);
            --argc;
            break;
         case 'i':
            i = atof(*++argv);
            --argc;
            break;
         case 'j':
            j = atof(*++argv);
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

   t = (j - i) / (double) l;

   for (;;) {
      if (freadf(&x2, sizeof(x2), 1, fp) != 1)
         break;
      if (freadf(&y2, sizeof(y2), 1, fp) != 1)
         break;

      fwritef(&y2, sizeof(y2), 1, stdout);

      for (end = 0, ii = 1; end == 0;) {
         x1 = x2;
         y1 = y2;
         if (freadf(&x2, sizeof(x2), 1, fp) != 1)
            break;
         if (freadf(&y2, sizeof(y2), 1, fp) != 1)
            break;

         if (x2 == j)
            end = 1;
         for (;; ii++) {
            x = i + t * ii;
            if (x > x2)
               break;
            y = ((y1 - y2) * x + x1 * y2 - x2 * y1) / (x1 - x2);
            fwritef(&y, sizeof(y), 1, stdout);
         }
      }
   }

   return (0);
}
