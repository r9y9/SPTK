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
/*                1996-2016  Nagoya Institute of Technology          */
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

    $Id: _uels.c,v 1.30 2016/12/22 10:53:12 fjst15124 Exp $

    Unbiased Estimation of Log Spectrum

        int uels(xw, flng, c, m, itr1, itr2, dd, etype, e, itype);

        double   *xw   : input sequence
        int      flng  : frame length
        double   *c    : cepstrum
        int      m     : order of cepstrum
        int      itr1  : minimum number of iteration
        int      itr2  : maximum number of iteration
        double   dd    : end condition
        int      etype :    0 -> e is not used
                            1 -> e is initial value for log-periodogram
                            2 -> e is floor periodogram in db
        double   e     : initial value for log-periodogram
                         or floor periodogram in db
        int      itype : input data type
        
        return   value :  0 -> completed by end condition
                          -1-> completed by maximum iteration

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

/* Fast Algorithm for Linear Prediction with Linear Phase */
static void lplp(double *r, double *c, const int m)
{
   int k, n;
   double pn, alpha, beta, gamma, tz = r[0] / 2, rtz = 1 / tz, to = r[1], rttz =
       0, tto = 1;
   static double *p = NULL, *pp;
   static int size;

   if (p == NULL) {
      p = dgetmem(m + m + 4);
      pp = p + m + 2;
      size = m;
   }
   if (m > size) {
      free(p);
      p = dgetmem(m + m + 2);
      pp = p + m + 1;
      size = m;
   }

   c[0] = 1.0 / r[0];
   p[0] = 1.0;
   p[1] = 0.0;
   pp[0] = 0.0;

   for (k = 1; k <= m; k++) {
      p[k + 1] = 0.0;

      pp[k] = 0.0;
      beta = -tz * rttz;
      alpha = tto * rttz;
      alpha -= (tto = to) * (rttz = rtz);
      pn = p[1] + p[1] + alpha * p[0] + beta * pp[0];
      pp[0] = p[0];
      p[0] = pn;

      for (n = 1; n <= k; n++) {
         pn = p[n + 1] + pp[n - 1] + alpha * p[n] + beta * pp[n];
         pp[n] = p[n];
         p[n] = pn;
      }
      for (n = 1, tz = p[0] * r[k]; n <= k; n++)
         tz += p[n] * (r[k - n] + r[k + n]);

      for (n = 1, to = p[0] * r[1 + k]; n <= k; n++)
         to += p[n] * (r[1 + k - n] + r[1 + k + n]);

      gamma = 0.5 * p[0] * (rtz = 1 / tz);

      for (n = 0; n < k; n++)
         c[n] = c[n] + gamma * p[n];

      c[k] = gamma * p[k];
   }
   for (c[0] = 1.0 / c[0], n = 1; n <= m; n++)
      c[n] *= c[0];

   return;
}

int uels(double *xw, const int flng, double *c, const int m, const int itr1,
         const int itr2, const double dd, const int etype, const double e,
         const int itype)
{
   int i, j, flag = 0;
   double k, eps = 0.0, min, max;
   static double *x = NULL, *r, *cr, *y, *a;
   static int size_x, size_a;

   if (etype == 1 && e < 0.0) {
      fprintf(stderr, "uels : value of e must be e>=0!\n");
      exit(1);
   }

   if (etype == 2 && e >= 0.0) {
      fprintf(stderr, "uels : value of E must be E<0!\n");
      exit(1);
   }

   if (etype == 1) {
      eps = e;
   }

   if (x == NULL) {
      x = dgetmem(4 * flng);
      a = dgetmem(m + 1);
      r = x + flng;
      cr = r + flng;
      y = cr + flng;
      size_x = flng;
      size_a = m;
   }
   if (flng > size_x) {
      free(x);
      x = dgetmem(4 * flng);
      r = x + flng;
      cr = r + flng;
      y = cr + flng;
      size_x = flng;
   }
   if (m > size_a) {
      free(a);
      a = dgetmem(m + 1);
      size_a = m;
   }

   movem(xw, x, sizeof(*xw), flng);

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
      fprintf(stderr, "uels : Input type %d is not supported!\n", itype);
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
      if (x[i] <= 0) {
         fprintf(stderr,
                 "uels : The log periodogram has '0', use '-e' option!\n");
         exit(1);
      }
      x[i] = cr[i] = log(x[i]);
   }
   ifftr(cr, y, flng);          /*  cr : c(m)  */

   /*  initial value  */
   k = exp(cr[0]);
   for (i = 1; i <= m; i++)
      c[i] = cr[i];

   for (j = 1; j <= itr2; j++) {
      cr[0] = 0.0;

      for (i = 1; i <= m; i++)
         cr[i] = c[i];
      for (; i < flng; i++)
         cr[i] = 0.0;

      fftr(cr, y, flng);        /*  cr+jy : log D(z)  */
      for (i = 0; i < flng; i++)
         r[i] = exp(x[i] - cr[i] - cr[i]);
      ifftr(r, y, flng);        /*  r : autocorr  */

      c[0] = k;
      k = r[0];

      if (j >= itr1) {
         if (fabs((k - c[0]) / c[0]) < dd) {
            flag = 1;
            break;
         }
         k = c[0];
      }

      lplp(r, a, m);
      for (i = 1; i <= m; i++)
         c[i] -= a[i];
   }

   c[0] = 0.5 * log(k);
   if (flag)
      return (0);
   else
      return (-1);
}
