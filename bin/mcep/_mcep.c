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

/****************************************************************

    $Id: _mcep.c,v 1.30 2013/12/16 09:02:00 mataki Exp $

    Mel-Cepstral Analysis

        int mcep(xw, flng, mc, m, a, itr1, itr2, dd, etype, e, f, itype);

        double   *xw   : input sequence
        int      flng  : frame length
        double   *mc   : mel cepstrum
        int      m     : order of mel cepstrum
        double   a     : alpha
        int      itr1  : minimum number of iteration
        int      itr2  : maximum number of iteration
        double   dd    : end condition
        int      etype :    0 -> e is not used
                            1 -> e is initial value for log-periodogram
                            2 -> e is floor periodogram in db
        double   e     : initial value for log-periodogram
                         or floor periodogram in db
        double   f     : mimimum value of the determinant 
                         of the normal matrix
        int      itype : input data type
                                
        return   value :    0 -> completed by end condition
                            -1-> completed by maximum iteration

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

int mcep(double *xw, const int flng, double *mc, const int m, const double a,
         const int itr1, const int itr2, const double dd, const int etype,
         const double e, const double f, const int itype)
{
   int i, j;
   int flag = 0, f2, m2;
   double t, s, eps = 0.0, min, max;
   static double *x = NULL, *y, *c, *d, *al, *b;
   static int size_x, size_d;

   if (etype == 1 && e < 0.0) {
      fprintf(stderr, "mcep : value of e must be e>=0!\n");
      exit(1);
   }

   if (etype == 2 && e >= 0.0) {
      fprintf(stderr, "mcep : value of E must be E<0!\n");
      exit(1);
   }

   if (etype == 1) {
      eps = e;
   }

   if (x == NULL) {
      x = dgetmem(3 * flng);
      y = x + flng;
      c = y + flng;
      size_x = flng;

      d = dgetmem(3 * m + 3);
      al = d + (m + 1);
      b = al + (m + 1);
      size_d = m;
   }
   if (flng > size_x) {
      free(x);
      x = dgetmem(3 * flng);
      y = x + flng;
      c = y + flng;
      size_x = flng;
   }
   if (m > size_d) {
      free(d);
      d = dgetmem(3 * m + 3);
      al = d + (m + 1);
      b = al + (m + 1);
      size_d = m;
   }

   f2 = flng / 2;
   m2 = m + m;

   movem(xw, x, sizeof(*x), flng);

   switch (itype) {
   case 0:                     /* windowed data sequence */
      fftr(x, y, flng);
      for (i = 0; i < flng; i++) {
         x[i] = x[i] * x[i] + y[i] * y[i] + eps;        /*  periodogram  */
      }
      break;
   case 1:                     /* dB */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = exp((x[i] / 20.0) * log(10.0)); /* dB -> amplitude spectrum */
         x[i] = x[i] * x[i] + eps;      /* amplitude -> periodogram */
      }
      break;
   case 2:                     /* log */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = exp(x[i]);      /* log -> amplitude spectrum */
         x[i] = x[i] * x[i] + eps;      /* amplitude -> periodogram */
      }
      break;
   case 3:                     /* amplitude */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = x[i] * x[i] + eps;      /* amplitude -> periodogram */
      }
      break;
   case 4:                     /* periodogram */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = x[i] + eps;
      }
      break;
   default:
      fprintf(stderr, "mcep : input type %d is not supported!\n", itype);
      exit(1);
   }
   if (itype > 0) {
      for (i = 1; i < flng / 2; i++)
         x[flng - i] = x[i];
   }

   if (etype == 2 && e < 0.0) {
      max = x[0];
      for (i = 1; i < flng; i++) {
         if (max < x[i])
            max = x[i];
      }
      max = sqrt(max);
      min = max * pow(10.0, e / 20.0);  /* floor is 20*log10(min/max) */
      min = min * min;
      for (i = 0; i < flng; i++) {
         if (x[i] < min)
            x[i] = min;
      }
   }

   for (i = 0; i < flng; i++) {
      if (x[i] <= 0.0) {
         fprintf(stderr,
                 "mcep : periodogram has '0', use '-e' option to floor it!\n");
         exit(1);
      }
      c[i] = log(x[i]);
   }

   /*  1, (-a), (-a)^2, ..., (-a)^M  */
   al[0] = 1.0;
   for (i = 1; i <= m; i++)
      al[i] = -a * al[i - 1];

   /*  initial value of cepstrum  */
   ifftr(c, y, flng);           /*  c : IFFT[x]  */

   c[0] /= 2.0;
   c[f2] /= 2.0;
   freqt(c, f2, mc, m, a);      /*  mc : mel cep.  */
   s = c[0];

   /*  Newton Raphson method  */
   for (j = 1; j <= itr2; j++) {
      fillz(c, sizeof(*c), flng);
      freqt(mc, m, c, f2, -a);  /*  mc : mel cep.  */
      fftr(c, y, flng);         /*  c, y : FFT[mc]  */
      for (i = 0; i < flng; i++)
         c[i] = x[i] / exp(c[i] + c[i]);
      ifftr(c, y, flng);
      frqtr(c, f2, c, m2, a);   /*  c : r(k)  */

      t = c[0];
      if (j >= itr1) {
         if (fabs((t - s) / t) < dd) {
            flag = 1;
            break;
         }
         s = t;
      }

      for (i = 0; i <= m; i++)
         b[i] = c[i] - al[i];
      for (i = 0; i <= m2; i++)
         y[i] = c[i];
      for (i = 0; i <= m2; i += 2)
         y[i] -= c[0];
      for (i = 2; i <= m; i += 2)
         c[i] += c[0];
      c[0] += c[0];

      if (theq(c, y, d, b, m + 1, f)) {
         fprintf(stderr, "mcep : Error in theq() at %dth iteration !\n", j);
         exit(1);
      }

      for (i = 0; i <= m; i++)
         mc[i] += d[i];
   }

   if (flag)
      return (0);
   else
      return (-1);

}

/***************************************************************

    Frequency Transformation for Calculating Coefficients

        void frqtr(c1, m1, c2, m2, a)

        double *c1   : minimum phase sequence
        int m1       : order of minimum phase sequence
        double *c2   : warped sequence
        int m2       : order of warped sequence
        double a     : all-pass constant

***************************************************************/

void frqtr(double *c1, int m1, double *c2, int m2, const double a)
{
   int i, j;
   static double *d = NULL, *g;
   static int size;

   if (d == NULL) {
      size = m2;
      d = dgetmem(size + size + 2);
      g = d + size + 1;
   }

   if (m2 > size) {
      free(d);
      size = m2;
      d = dgetmem(size + size + 2);
      g = d + size + 1;
   }

   fillz(g, sizeof(*g), m2 + 1);

   for (i = -m1; i <= 0; i++) {
      if (0 <= m2) {
         d[0] = g[0];
         g[0] = c1[-i];
      }
      for (j = 1; j <= m2; j++)
         g[j] = d[j - 1] + a * ((d[j] = g[j]) - g[j - 1]);
   }

   movem(g, c2, sizeof(*g), m2 + 1);

   return;
}
