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
/*                1996-2017  Nagoya Institute of Technology          */
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

    $Id$

    Adaptive Cepstral Analysis

    double acep(x, c, m, lambda, step, pd);

    double      x : input sequence
    double     *c : cepstrum
    int         m : order of cepstrum
    double lambda : leakage factor
    double   step : step size
    int        pd : order of Pade approximation
    double    eps : minimum value for epsilon

    return value  : prediction error

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

double acep(double x, double *c, const int m, const double lambda,
            const double step, const double tau, const int pd, const double eps)
{
   int i, memory_size;
   static double *cc = NULL, *e, *ep, *d, gg = 1.0;
   static int size;
   double mu, tx;

   memory_size = m + m + m + 3 + (m + 1) * pd * 2;
   if (cc == NULL) {
      cc = dgetmem(memory_size);
      e = cc + m + 1;
      ep = e + m + 1;
      d = ep + m + 1;
      size = memory_size;
   }

   if (memory_size > size) {
      free(cc);
      cc = dgetmem(memory_size);
      e = cc + m + 1;
      ep = e + m + 1;
      d = ep + m + 1;
      size = memory_size;
   }

   for (i = 1; i <= m; i++)
      cc[i] = -c[i];

   x = lmadf(x, cc, m, pd, d);

   for (i = m; i >= 1; i--)
      e[i] = e[i - 1];
   e[0] = x;

   gg = gg * lambda + (1.0 - lambda) * e[0] * e[0];
   c[0] = 0.5 * log(gg);

   gg = (gg < eps) ? eps : gg;
   mu = step / (double) m / gg;
   tx = 2 * (1.0 - tau) * x;

   for (i = 1; i <= m; i++) {
      ep[i] = tau * ep[i] - tx * e[i];
      c[i] -= mu * ep[i];
   }

   return (x);
}
