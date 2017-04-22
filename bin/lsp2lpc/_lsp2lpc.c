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

    $Id: _lsp2lpc.c,v 1.19 2016/12/22 10:53:07 fjst15124 Exp $

    Transformation LSP to LPC

        void lsp2lpc(lsp, a, m)

        double  *lsp : LSP
        double  *a   : LPC
        int      m   : order of LPC

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

void lsp2lpc(double *lsp, double *a, const int m)
{
   int i, k, mh1, mh2, flag_odd;
   double xx, xf, xff;
   static double *f = NULL, *p, *q, *a0, *a1, *a2, *b0, *b1, *b2;
   static int size;

   flag_odd = 0;
   if (m % 2 == 0)
      mh1 = mh2 = m / 2;
   else {
      mh1 = (m + 1) / 2;
      mh2 = (m - 1) / 2;
      flag_odd = 1;
   }

   if (f == NULL) {
      f = dgetmem(5 * m + 6);
      p = f + m;
      q = p + mh1;
      a0 = q + mh2;
      a1 = a0 + (mh1 + 1);
      a2 = a1 + (mh1 + 1);
      b0 = a2 + (mh1 + 1);
      b1 = b0 + (mh2 + 1);
      b2 = b1 + (mh2 + 1);
      size = m;
   }
   if (m > size) {
      free(f);
      f = dgetmem(5 * m + 6);
      p = f + m;
      q = p + mh1;
      a0 = q + mh2;
      a1 = a0 + (mh1 + 1);
      a2 = a1 + (mh1 + 1);
      b0 = a2 + (mh1 + 1);
      b1 = b0 + (mh2 + 1);
      b2 = b1 + (mh2 + 1);
      size = m;
   }

   movem(lsp, f, sizeof(*lsp), m);

   fillz(a0, sizeof(*a0), mh1 + 1);
   fillz(b0, sizeof(*b0), mh2 + 1);
   fillz(a1, sizeof(*a1), mh1 + 1);
   fillz(b1, sizeof(*b1), mh2 + 1);
   fillz(a2, sizeof(*a2), mh1 + 1);
   fillz(b2, sizeof(*b2), mh2 + 1);

   /* lsp filter parameters */
   for (i = k = 0; i < mh1; i++, k += 2)
      p[i] = -2.0 * cos(PI2 * f[k]);
   for (i = k = 0; i < mh2; i++, k += 2)
      q[i] = -2.0 * cos(PI2 * f[k + 1]);

   /* impulse response of analysis filter */
   xx = 1.0;
   xf = xff = 0.0;
   for (k = 0; k <= m; k++) {
      if (flag_odd) {
         a0[0] = xx;
         b0[0] = xx - xff;
         xff = xf;
         xf = xx;
      } else {
         a0[0] = xx + xf;
         b0[0] = xx - xf;
         xf = xx;
      }

      for (i = 0; i < mh1; i++) {
         a0[i + 1] = a0[i] + p[i] * a1[i] + a2[i];
         a2[i] = a1[i];
         a1[i] = a0[i];
      }
      for (i = 0; i < mh2; i++) {
         b0[i + 1] = b0[i] + q[i] * b1[i] + b2[i];
         b2[i] = b1[i];
         b1[i] = b0[i];
      }

      if (k != 0)
         a[k - 1] = -0.5 * (a0[mh1] + b0[mh2]);

      xx = 0.0;
   }

   for (i = m - 1; i >= 0; i--)
      a[i + 1] = -a[i];
   a[0] = 1.0;

   return;
}
