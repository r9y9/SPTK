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

/****************************************************************

    $Id: _gcep.c,v 1.23 2014/12/11 08:30:36 uratec Exp $

    Generalized Cepstral Analysis

        int gcep(xw, flng, gc, m, g, itr1, itr2, d, etype, e, f, itype);

        double    *xw   : input sequence
        int       flng  : frame length
        double    *gc   : generalized cepstrum
        int       m     : order of generalized cepstrum
        double    g     : gamma
        int       itr1  : minimum number of iteration
        int       itr2  : maximum number of iteration
        double    d     : end condition
        int       etype :    0 -> e is not used
                             1 -> e is initial value for log-periodogram
                             2 -> e is floor periodogram in db
        double    e     : initial value for log-periodogram
                          or floor periodogram in db
        double    f     : mimimum value of the determinant
                         of the normal matrix
        int      itype : input data type

        return value :    0 -> completed by end condition
                          -1-> completed by maximum iteration
                          1 -> invalid etype
                          2 -> invalid itype
                          3 -> failed to compute generalized cepstrum

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

int gcep(double *xw, const int flng, double *gc, const int m, const double g,
         const int itr1, const int itr2, const double d, const int etype,
         const double e, const double f, const int itype)
{
   int i, j, flag = 0;
   double t, s, eps = 0.0, min, max, dd = 0.0;
   static double *x = NULL, *y, *cr, *ci, *rr, *hr, *hi, *er, *ei;
   static int size;

   if (etype == 1 && e < 0.0) {
      fprintf(stderr, "gcep : value of e must be e>=0!\n");
      return 1;
   }

   if (etype == 2 && e >= 0.0) {
      fprintf(stderr, "gcep : value of E must be E<0!\n");
      return 1;
   }

   if (etype == 1) {
      eps = e;
   }

   if (x == NULL) {
      x = dgetmem(9 * flng);
      size = flng;

      y = x + flng;
      cr = y + flng;
      ci = cr + flng;
      rr = ci + flng;
      hr = rr + flng;
      hi = hr + flng;
      er = hi + flng;
      ei = er + flng;
   }

   if (flng > size) {
      free(x);
      x = dgetmem(9 * flng);
      size = flng;

      y = x + flng;
      cr = y + flng;
      ci = cr + flng;
      rr = ci + flng;
      hr = rr + flng;
      hi = hr + flng;
      er = hi + flng;
      ei = er + flng;
   }

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
      fprintf(stderr, "gcep : Input type %d is not supported!\n", itype);
      return 2;
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

   for (i = 0; i < flng; i++)
      cr[i] = log(x[i]);

   /*  initial value of generalized cepstrum  */
   ifftr(cr, y, flng);          /*  x : IFFT[x]  */
   cr[0] = exp(cr[0] / 2);
   gc2gc(cr, m, 0.0, gc, m, g); /*  gc : generalized cepstrum  */

   /*  Newton-Raphson method  */
   for (j = 1; j <= itr2; j++) {
      fillz(cr, sizeof(*cr), flng);
      movem(&gc[1], &cr[1], sizeof(*cr), m);
      fftr(cr, ci, flng);       /*  cr+jci : FFT[gc]  */

      for (i = 0; i < flng; i++) {
         t = x[i] / agexp(g, cr[i], ci[i]);
         cr[i] = 1 + g * cr[i];
         ci[i] = g * ci[i];
         s = cr[i] * cr[i] + ci[i] * ci[i];
         rr[i] = t / s;
         hr[i] = (cr[i] * cr[i] - ci[i] * ci[i]) * t / (s * s);
         hi[i] = 2 * cr[i] * ci[i] * t / (s * s);
         er[i] = cr[i] * t / s;
         ei[i] = ci[i] * t / s;
      }

      ifftr(rr, y, flng);       /*  rr : r(k)  */
      ifft(hr, hi, flng);       /*  hr : h(k)  */
      ifft(er, ei, flng);       /*  er : e(k)  */
      s = gc[0];                /*  gc[0] : gain  */

      for (i = 1, t = 0.0; i <= m; i++)
         t += er[i] * gc[i];

      t = er[0] + g * t;
      t = sqrt(fabs(t));

      if (j >= itr1) {
         if (fabs((t - dd) / t) < d) {
            flag = 1;
            break;
         }
         dd = t;
      }

      for (i = 2; i <= m + m; i++)
         hr[i] *= 1 + g;

      if (theq(rr, &hr[2], &y[1], &er[1], m, f)) {
         fprintf(stderr, "gcep : Error in theq() at %dth iteration!\n", j);
         return 3;
      }

      gc[0] = t;

      for (i = 1; i <= m; i++)
         gc[i] += y[i];
   }

   if (flag)
      return (0);
   else
      return (-1);
}
