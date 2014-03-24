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

    $Id: _agcep.c,v 1.16 2013/12/16 09:01:53 mataki Exp $

    Adaptive Generalized Cepstral Analysis

    double agcep(x, c, m, lambda, step);

    double x      : input sequence
    double *c     : normalized generalized cepstrum
    int    m      : order of generalized cepstrum
    int    stage  : -1 / gamma
    double lambda : leakage factor
    double step   : step size
    double tau    : momentum constant
    double eps    : minimum value for epsilon

    return value  : prediction error

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

double agcep(double x, double *c, const int m, const int stage,
             const double lambda, const double step, const double tau,
             const double eps)
{
   int i;
   static double *eg = NULL, *ep, *d, gg = 1.0, ee = 1.0, tx;
   static int size;
   double mu, ll;

   if (eg == NULL) {
      eg = dgetmem(2 * (m + 1) + m * stage);
      ep = eg + m + 1;
      d = ep + m + 1;
      size = m;
   }
   if (m > size) {
      free(eg);
      eg = dgetmem(2 * (m + 1) + m * stage);
      ep = eg + m + 1;
      d = ep + m + 1;
      size = m;
   }

   ll = 1.0 - lambda;

   eg[m] = d[stage * m - 1];
   x = iglsadf1(x, c, m, stage, d);

   movem(d + (stage - 1) * m, eg, sizeof(*d), m);

   gg = lambda * gg + ll * eg[0] * eg[0];
   gg = (gg < eps) ? eps : gg;
   mu = step / (double) m / gg;
   tx = 2 * (1.0 - tau) * x;

   for (i = 1; i <= m; i++) {
      ep[i] = tau * ep[i] - tx * eg[i];
      c[i] -= mu * ep[i];
   }

   ee = lambda * ee + ll * x * x;
   c[0] = sqrt(ee);

   return (x);
}
