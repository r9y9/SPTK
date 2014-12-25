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
*    Adaptive Cepstral Analysis                                         *
*                                                                       *
*                                       1993.8  K.Tokuda                *
*                                       1996.1  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               acep [ options ] [ pefile ] < stdin > stdout            *
*       options:                                                        *
*               -m m     :  order of cepstrum           [25]            *
*               -l l     :  leakage factor              [0.98]          *
*               -t t     :  momentum constant           [0.9]           *
*               -k k     :  step size                   [0.1]           *
*               -p p     :  output period of cepstrum   [1]             *
*               -s       :  output smoothed cepstrum    [FALSE]         *
*               -e e     :  minimum value for epsilon   [0]             *
*               -P P     :  order of Pade approximation [4]             *
*       infile:                                                         *
*               data sequence                                           *
*                   , x(0), x(1), ...                                   *
*       stdout:                                                         *
*               cepstrum                                                *
*                   , c(0), c(1), ..., c(M),                            *
*       output:                                                         *
*               prediction error (if pefile is specified)               *
*                   , e(0), e(1), ...                                   *
*       notice:                                                         *
*               P = 4 or 5                                              *
*       require:                                                        *
*               lmadf()                                                 *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: acep.c,v 1.27 2014/12/11 08:30:28 uratec Exp $";


/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
#define PADEORD 4
#define LAMBDA 0.98
#define STEP 0.1
#define PERIOD 1
#define AVEFLAG FA
#define TAU 0.9
#define EPS 0.0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(const int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - adaptive cepstral analysis\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ pefile ] < stdin > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of cepstrum           [%d]\n", ORDER);
   fprintf(stderr, "       -l l  : leakage factor              [%g]\n", LAMBDA);
   fprintf(stderr, "       -t t  : momentum constant           [%g]\n", TAU);
   fprintf(stderr, "       -k k  : step size                   [%g]\n", STEP);
   fprintf(stderr, "       -p p  : output period of cepstrum   [%d]\n", PERIOD);
   fprintf(stderr, "       -s    : output smoothed cepstrum    [%s]\n",
           BOOL[AVEFLAG]);
   fprintf(stderr, "       -e e  : minimum value for epsilon   [%g]\n", EPS);
   fprintf(stderr, "       -P P  : order of Pade approximation [%d]\n",
           PADEORD);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  stdin:\n");
   fprintf(stderr, "       data sequence (%s)\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       cepstrum (%s)\n", FORMAT);
   fprintf(stderr, "  pefile:\n");
   fprintf(stderr, "       prediction error (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       P = 4 or 5\n");
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
   int m = ORDER, period = PERIOD, i, j, pd = PADEORD;
   FILE *fp = stdin, *fpe = NULL;
   Boolean aveflag = AVEFLAG;
   double lambda = LAMBDA, step = STEP, tau = TAU, eps = EPS,
       *c, *e, *ep, *cc, *d, *avec, x, ll, gg, tt, mu, ttx;

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
         case 'p':
            period = atoi(*++argv);
            --argc;
            break;
         case 's':
            aveflag = 1 - aveflag;
            break;
         case 'P':
            pd = atoi(*++argv);
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
         fpe = getfp(*argv, "wb");

   if ((pd < 4) || (pd > 5)) {
      fprintf(stderr, "%s : Order of Pade approximation should be 4 or 5!\n",
              cmnd);
      return (1);
   }

   c = dgetmem(5 * (m + 1) + (m + 1) * pd * 2);
   cc = c + m + 1;
   e = cc + m + 1;
   ep = e + m + 1;
   avec = ep + m + 1;
   d = avec + m + 1;

   j = period;
   ll = 1.0 - lambda;
   gg = 1.0;
   step /= (double) m;
   tt = 2 * (1.0 - tau);

   while (freadf(&x, sizeof(x), 1, fp) == 1) {
      for (i = 1; i <= m; i++)
         cc[i] = -c[i];

      x = lmadf(x, cc, m, pd, d);

      for (i = m; i >= 1; i--)
         e[i] = e[i - 1];
      e[0] = x;

      gg = gg * lambda + ll * e[0] * e[0];
      c[0] = 0.5 * log(gg);

      gg = (gg < eps) ? eps : gg;
      mu = step / gg;
      ttx = tt * e[0];

      for (i = 1; i <= m; i++) {
         ep[i] = tau * ep[i] - ttx * e[i];
         c[i] -= mu * ep[i];
      }

      if (aveflag)
         for (i = 0; i <= m; i++)
            avec[i] += c[i];

      if (fpe != NULL)
         fwritef(&x, sizeof(x), 1, fpe);

      if (--j == 0) {
         j = period;
         if (aveflag) {
            for (i = 0; i <= m; i++)
               avec[i] /= period;
            fwritef(avec, sizeof(*avec), m + 1, stdout);
            fillz(avec, sizeof(*avec), m + 1);
         } else
            fwritef(c, sizeof(*c), m + 1, stdout);
      }
   }
   return (0);
}
