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

/******************************************************************************
*                                                                             *
*    Adaptive Generalized Cepstral Analysis                                   *
*                                                                             *
*                                       1991.11 K.Tokuda                      *
*                                       1996.1  K.Koishida                    *
*                                                                             *
*       usage:                                                                *
*               agcep [ options ] [ pefile ] < stdin > stdout                 *
*       options:                                                              *
*               -m m     :  order of generalized cepstrum          [25]       *
*               -c c     :  gamma = -1 / (int) c                   [1]        *
*               -l l     :  leakage factor                         [0.98]     *
*               -t t     :  momentum constant                      [0.9]      *
*               -k k     :  step size                              [0.1]      *
*               -p p     :  output period of generalized cepstrum  [1]        *
*               -s       :  output smoothed generalized cepstrum   [FALSE]    *
*               -n       :  output normalized generalized cepstrum [FALSE]    *
*               -e e     :  minimum value for epsilon              [0]        *
*       infile:                                                               *
*               data sequence                                                 *
*                   , x(0), x(1), ...                                         *
*       stdout:                                                               *
*               generalized cepstrum                                          *
*                   , c(0), c(1), ..., c(m),                                  *
*       output:                                                               *
*               prediction error (if pefile is specified)                     *
*                   , e(0), e(1), ...                                         *  
*       require:                                                              *
*               iglsadf1(), ignorm()                                          *
*                                                                             *
******************************************************************************/

static char *rcs_id = "$Id: agcep.c,v 1.29 2013/12/16 09:01:53 mataki Exp $";


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
#define STAGE 1
#define LAMBDA 0.98
#define STEP 0.1
#define PERIOD 1
#define AVERAGE FA
#define NORM FA
#define TAU 0.9
#define EPS 0.0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - adaptive generalized cepstral analysis\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ pefile ] < stdin > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m  : order of generalized cepstrum          [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -c c  : gamma = -1 / (int) c                   [%d]\n",
           STAGE);
   fprintf(stderr,
           "       -l l  : leakage factor                         [%g]\n",
           LAMBDA);
   fprintf(stderr,
           "       -t t  : momentum constant                      [%g]\n", TAU);
   fprintf(stderr,
           "       -k k  : step size                              [%g]\n",
           STEP);
   fprintf(stderr,
           "       -p p  : output period of generalized cepstrum  [%d]\n",
           PERIOD);
   fprintf(stderr,
           "       -s    : output smoothed generalized cepstrum   [%s]\n",
           BOOL[AVERAGE]);
   fprintf(stderr,
           "       -n    : output normalized generalized cepstrum [%s]\n",
           BOOL[NORM]);
   fprintf(stderr,
           "       -e e  : minimum value for epsilon              [%g]\n", EPS);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  stdin:\n");
   fprintf(stderr, "       data sequence (%s)\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       generalized cepstrum (%s)\n", FORMAT);
   fprintf(stderr, "  pefile:\n");
   fprintf(stderr, "       prediction error (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif                          /* PACKAGE_VERSION */
   fprintf(stderr, "\n");
   exit(status);
}


int main(int argc, char **argv)
{
   int m = ORDER, period = PERIOD, stage = STAGE, i, j;
   FILE *fp = stdin, *fpe = NULL;
   Boolean ave = AVERAGE, norm = NORM;
   double lambda = LAMBDA, step = STEP, eps = EPS,
       *c, *cc, *eg, *ep, *d, *avec, tau = TAU,
       x, ee, ll, gg, mu, gamma, tt, ttx;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            lambda = atof(*++argv);
            --argc;
            break;
         case 't':
            tau = atof(*++argv);
            --argc;
            break;
         case 'k':
            step = atof(*++argv);
            --argc;
            break;
         case 'm':
            m = atoi(*++argv);
            --argc;
            break;
         case 'c':
            stage = atoi(*++argv);
            --argc;
            break;
         case 'p':
            period = atoi(*++argv);
            --argc;
            break;
         case 's':
            ave = 1 - ave;
            break;
         case 'n':
            norm = 1 - norm;
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
         fpe = getfp(*argv, "wb");

   if (stage == 0) {
      fprintf(stderr, "%s : gamma should not equal to 0!\n", cmnd);
      usage(1);
   }
   gamma = -1.0 / (double) stage;

   c = dgetmem(5 * (m + 1) + m * stage);
   cc = c + m + 1;
   eg = cc + m + 1;
   ep = eg + m + 1;
   avec = ep + m + 1;
   d = avec + m + 1;

   j = period;
   ll = 1.0 - lambda;
   gg = 1.0;
   ee = 1.0;
   step /= (double) m;
   tt = 2 * (1.0 - tau);

   while (freadf(&x, sizeof(x), 1, fp) == 1) {
      eg[m] = d[stage * m - 1];
      x = iglsadf1(x, c, m, stage, d);

      movem(d + (stage - 1) * m, eg, sizeof(*d), m);

      gg = lambda * gg + ll * eg[0] * eg[0];
      gg = (gg < eps) ? eps : gg;
      mu = step / gg;
      ttx = tt * x;

      for (i = 1; i <= m; i++) {
         ep[i] = tau * ep[i] - ttx * eg[i];
         c[i] -= mu * ep[i];
      }

      ee = lambda * ee + ll * x * x;
      c[0] = sqrt(ee);

      if (ave)
         for (i = 0; i <= m; i++)
            avec[i] += c[i];

      if (fpe != NULL)
         fwritef(&x, sizeof(x), 1, fpe);

      if (--j == 0) {
         j = period;
         if (ave) {
            for (i = 0; i <= m; i++)
               avec[i] /= period;
            if (!norm)
               ignorm(avec, cc, m, gamma);
            fwritef(cc, sizeof(*cc), m + 1, stdout);
            fillz(avec, sizeof(*avec), m + 1);
         } else if (!norm) {
            ignorm(c, cc, m, gamma);
            fwritef(cc, sizeof(*cc), m + 1, stdout);
         } else
            fwritef(c, sizeof(*c), m + 1, stdout);
      }
   }
   return (0);
}
