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
*    FFT Cepstral Analysis                                              *
*                                                                       *
*                                       1985.11 K.Tokuda                *
*                                       1996.3  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               fftcep [ options ] [ infile ] > stdout                  *
*       options:                                                        *
*               -m  m    :  order of cepstrum             [25]          *
*               -l  l    :  frame length                  [256]         *
*               -j  j    :  number of iteration           [0]           *
*               -k  k    :  acceleration factor           [0]           *
*               -e  e    :  epsilon                       [0]           *
*       infile:                                                         *
*               data sequence                                           *
*                   , x(0), x(1), ..., x(l-1),                          *
*       stdout:                                                         *
*               cepstral coefficients                                   *
*                   , c(0), c(1), ..., c(m),                            *
*       require:                                                        *
*               fftcep()                                                *
*       notice:                                                         *
*               When -j & -k options are specified,                     *
*               improved cepstral analysis is performed.                *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: fftcep.c,v 1.30 2014/12/11 08:30:34 uratec Exp $";


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
#define FLNG 256
#define MAXITR 0
#define ACCELERATION 0.0
#define EPS 0.0

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - FFT cepstral analysis\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of cepstrum   [%d]\n", ORDER);
   fprintf(stderr, "       -l l  : frame length        [%d]\n", FLNG);
   fprintf(stderr, "       -j j  : number of iteration [%d]\n", MAXITR);
   fprintf(stderr, "       -k k  : acceleration factor [%g]\n", ACCELERATION);
   fprintf(stderr, "       -e e  : epsilon             [%g]\n", EPS);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       windowed sequence (%s)   [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       cepstrum (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       When -j & -k options are specified,\n");
   fprintf(stderr, "       improved cepstral analysis is performed.\n");
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
   int m = ORDER, l = FLNG, itr = MAXITR, i;
   double ac = ACCELERATION, eps = EPS, *x, *y, *c;
   FILE *fp = stdin;

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
         case 'j':
            itr = atoi(*++argv);
            --argc;
            break;
         case 'k':
            ac = atof(*++argv);
            --argc;
            break;
         case 'e':
            eps = atof(*++argv);
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

   x = dgetmem(l + l + m + 1);
   y = x + l;
   c = y + l;

   while (freadf(x, sizeof(*x), l, fp) == l) {
      fftr(x, y, l);

      for (i = 0; i < l; i++)
         x[i] = log(x[i] * x[i] + y[i] * y[i] + eps);

      fftcep(x, l, c, m, itr, ac);

      fwritef(c, sizeof(*c), m + 1, stdout);
   }

   return 0;
}
