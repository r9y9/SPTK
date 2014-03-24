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
*    LPC Analysis Using Levinson-Durbin Method                          *
*                                                                       *
*                                    1996.1  K.Koishida                 *
*                                                                       *
*       usage:                                                          *
*               lpc [ options ] [ infile ] > stdout                     *
*       options:                                                        *
*               -l l  :  frame length                      [256]        *
*               -m m  :  order of LPC                      [25]         *
*               -f f  :  mimimum value of the determinant               *
*                        of the normal matrix              [0.000001]   *
*       infile:                                                         *
*               data sequence                                           *
*                       , x(0), x(1), ..., x(l-1),                      *
*       stdout:                                                         *
*               LP Coefficeints                                         *
*                       , K, a(1), ..., a(m),                           *
*       require:                                                        *
*               lpc()                                                   *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: lpc.c,v 1.26 2013/12/16 09:01:59 mataki Exp $";


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
#define ORDER 25
#define FLNG 256
#define MINDET 0.000001

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - LPC analysis using Levinson-Durbin method\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : frame length                     [%d]\n",
           FLNG);
   fprintf(stderr, "       -m m  : order of LPC                     [%d]\n",
           ORDER);
   fprintf(stderr, "       -f f  : mimimum value of the determinant [%g]\n",
           MINDET);
   fprintf(stderr, "               of the normal matrix\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       windowed sequence (%s)                [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       LP coefficients (%s)\n", FORMAT);
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
   int m = ORDER, l = FLNG, flag, t = 0;
   FILE *fp = stdin;
   double *x, *a, f = MINDET;

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
            l = atoi(*++argv);
            --argc;
            break;
         case 'f':
            f = atof(*++argv);
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

   x = dgetmem(l + m + 1);
   a = x + l;

   while (freadf(x, sizeof(*x), l, fp) == l) {
      flag = lpc(x, l, a, m, f);
      switch (flag) {
      case -1:
         fprintf(stderr,
                 "%s : The coefficient matrix of the normal equation is singular at %dth frame!\n",
                 cmnd, t);
         exit(1);
         break;
      case -2:
         fprintf(stderr,
                 "%s : Extracted LPC coefficients become unstable at %dth frame!\n",
                 cmnd, t);
         break;
      }

      fwritef(a, sizeof(*a), m + 1, stdout);
      t++;
   }

   return (0);
}
