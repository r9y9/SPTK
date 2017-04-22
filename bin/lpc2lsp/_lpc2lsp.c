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

    $Id: _lpc2lsp.c,v 1.18 2016/12/22 10:53:06 fjst15124 Exp $

    Transformation LPC to LSP

        int    lpc2lsp(lpc, lsp, order, numsp, maxp, maxq, eps)

        double *lpc   : LPC
        double *lsp   : LSP
        int    order  : order of LPC
        int    numsp  : split number of unit circle
        int    maxitr : maximum number of interpolation
        double eps    : end condition for interpolation

        return value
                   0  : completed normally
                   -1 : completed irregularly

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

static double chebpoly(const double x, double *c, const int mh)
{
   int i;
   double b[3];

   b[1] = b[2] = 0.0;
   for (i = mh; i > 0; i--) {
      b[0] = 2.0 * x * b[1] - b[2] + c[i];
      b[2] = b[1];
      b[1] = b[0];
   }
   b[0] = x * b[1] - b[2] + c[0];

   return (b[0]);
}

int lpc2lsp(double *lpc, double *lsp, const int order, const int numsp,
            const int maxitr, const double eps)
{
   int i;
   double *p1, *p2;
   int mh1, mh2, mh, mm, itr, flag_odd;
   double delta, x0, x1, g0, g1, x, y;
   static double *c1 = NULL, *c2;
   static int size_order;

   delta = 1.0 / (double) numsp;

   flag_odd = 0;
   if (order % 2 == 0)
      mh1 = mh2 = order / 2;
   else {
      mh1 = (order + 1) / 2;
      mh2 = (order - 1) / 2;
      flag_odd = 1;
   }

   if (c1 == NULL) {
      c1 = dgetmem(2 * (mh1 + 1));
      c2 = c1 + (mh1 + 1);
      size_order = order;
   }
   if (order > size_order) {
      free(c1);
      c1 = dgetmem(2 * (mh1 + 1));
      c2 = c1 + (mh1 + 1);
      size_order = order;
   }

   /* calculate symmetric and antisymmetrica polynomials */
   p1 = lpc + 1;
   p2 = lpc + order;
   c1[mh1] = c2[mh2] = 1.0;
   if (flag_odd) {
      c2[mh2 + 1] = 0.0;
      for (i = mh2 - 1; i >= 0; i--) {
         c1[i + 1] = *p1 + *p2;
         c2[i] = *p1++ - *p2-- + c2[i + 2];
      }
      c1[0] = *p1 + *p2;
   } else {
      for (i = mh1 - 1; i >= 0; i--) {
         c1[i] = *p1 + *p2 - c1[i + 1];
         c2[i] = *p1++ - *p2-- + c2[i + 1];
      }
   }
   c1[0] *= 0.5;
   c2[0] *= 0.5;

   /* root search */
   p1 = c1;
   mh = mh1;
   g0 = chebpoly(1.0, p1, mh);

   mm = 0;
   for (x = 1.0 - delta; x > -delta - 1.0; x -= delta) {
      g1 = chebpoly(x, p1, mh);

      if (g0 * g1 <= 0.0) {
         x0 = x + delta;
         x1 = x;

         itr = 0;
         do {
            x = (x0 + x1) / 2.0;
            y = chebpoly(x, p1, mh);

            if (y * g0 < 0.0) {
               x1 = x;
               g1 = y;
            } else {
               x0 = x;
               g0 = y;
            }

            itr++;
         }
         while ((fabs(y) > eps) && (itr < maxitr));

         x = (g1 * x0 - g0 * x1) / (g1 - g0);
         lsp[mm] = acos(x) / PI2;

         mm++;
         if (mm == order)
            return (0);

         if (p1 == c1) {
            p1 = c2;
            mh = mh2;
         } else {
            p1 = c1;
            mh = mh1;
         }

         g1 = chebpoly(x, p1, mh);
      }
      g0 = g1;
   }
   return (-1);
}
