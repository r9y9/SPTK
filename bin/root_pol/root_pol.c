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
*    Durand-Kerner-Aberth for Higher Order Equation                     *
*                                                                       *
*                                         1988.12  K.Fukushi            *
*                                         1989.3   T.Kobayashi modified *
*                                         1999.12  T.Yoshimura modified *
*                                                                       *
*       usage :                                                         *
*               root_pol [ options ] [ infile ] > stdout                *
*       options:                                                        *
*               -m  : order of equation              [32]               *
*               -n  : number of maximum iteration    [1000]             *
*               -e  : eps                            [1.0e-14]          *
*               -i  : a[0] = 1                       [FALSE]            *
*               -s  : revert the coefficient         [FALSE]            *
*               -r  : ( magnitude, arg ) output      [(Real,Imag)]      *
*               Pn(x) = a[0]x^n + a[1]x^n-1 + ... + a[n-1]x + a[n]      *
*      infile:                                                          *
*               stdin for default                                       *
*                       a[0],...,a[n]                                   *
*      outfile:                                                         *
*                       x[0].re, x[1].re,...,x[n-1].re,                 *
*                       x[0].im, x[1].im,...,x[n-1].im                  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: root_pol.c,v 1.25 2013/12/16 09:02:03 mataki Exp $";


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

/* Default Values */
#define ORDER 32
#define ITER  1000
#define EPS   1.0e-14

/* Command Name */
char *cmnd;


int usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - Durand-Kerner-Aberth for higher order equation \n",
           cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m  : order of equation            [%d]\n", ORDER);
   fprintf(stderr, "       -n  : number of maximum iteration  [%d]\n", ITER);
   fprintf(stderr, "       -e  : error margin for roots       [%g]\n", EPS);
   fprintf(stderr, "       -i  : a[0] = 1                     [FALSE]\n");
   fprintf(stderr, "       -s  : revert the coefficient       [FALSE]\n");
   fprintf(stderr, "       -r  : (magnitude, arg) output      [(Real,Imag)]\n");
   fprintf(stderr, "       -h  : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       coefficients of polynomial (%s) [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       root of polynomial (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(1);
}


int main(int argc, char *argv[])
{
   char *s;
   int c, k, odr = ORDER, itrat = ITER, form = 0, a_zero = 0;
   double *a, *d, eps = EPS;
   FILE *fp = stdin;
   complex *x;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc > 0) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 'i':
            a_zero = 1;
            break;
         case 'r':
            form |= 1;
            break;
         case 's':
            form |= 2;
            break;
         case 'e':
            if (*++s == '\0') {
               s = *++argv;
               --argc;
            }
            eps = atof(s);
            break;
         case 'm':
            if (*++s == '\0') {
               s = *++argv;
               --argc;
            }
            odr = atoi(s);
            break;
         case 'n':
            if (*++s == '\0') {
               s = *++argv;
               --argc;
            }
            itrat = atoi(s);
            break;
         default:
            usage();
         }
      } else
         fp = getfp(*argv, "rb");
   }

   a = dgetmem(odr + 1);
   odr = freadf(a, sizeof(*a), odr, fp) - 1;

   d = dgetmem(odr + 1);
   x = cplx_getmem(odr + 1);

   for (k = 0; k <= odr; ++k)
      d[k] = a[k];

   root_pol(d, odr, x, a_zero, eps, itrat);
   output_root_pol(x, odr, form);

   return (0);
}
