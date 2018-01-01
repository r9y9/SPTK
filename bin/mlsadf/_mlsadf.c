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

    MLSA Digital Filter

        double mlsadf(x, b, m, a, pd, d)

        double x     : input
        double *c    : MLSA filter coefficients
        int    m     : order of cepstrum
        double a     : all-pass constant
        int    pd    : order of Pade approximation
        double *d    : delay

        return value : filtered data

*****************************************************************/

#include <stdio.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

static double pade[] = { 1.0,
   1.0, 0.0,
   1.0, 0.0, 0.0,
   1.0, 0.0, 0.0, 0.0,
   1.0, 0.4999273, 0.1067005, 0.01170221, 0.0005656279,
   1.0, 0.4999391, 0.1107098, 0.01369984, 0.0009564853, 0.00003041721,
   1.0, 0.500000157834843, 0.113600112846183, 0.015133367945131, 0.001258740956606, 0.000062701416552, 0.000001481891776,
   1.0, 0.499802889651314, 0.115274789205577, 0.015997611632083, 0.001452640362652, 0.000087007832645, 0.000003213962732, 0.000000057148619
};

double *ppade;

static double mlsafir(double x, double *b, const int m, const double a,
                      double *d)
{
   double y = 0.0, aa;
   int i;

   aa = 1 - a * a;

   d[0] = x;
   d[1] = aa * d[0] + a * d[1];

   for (i = 2; i <= m; i++) {
      d[i] = d[i] + a * (d[i + 1] - d[i - 1]);
      y += d[i] * b[i];
   }

   for (i = m + 1; i > 1; i--)
      d[i] = d[i - 1];

   return (y);
}

static double mlsadf1(double x, double *b, const double a,
                      const int pd, double *d)
{
   double v, out = 0.0, *pt, aa;
   int i;

   aa = 1 - a * a;
   pt = &d[pd + 1];

   for (i = pd; i >= 1; i--) {
      d[i] = aa * pt[i - 1] + a * d[i];
      pt[i] = d[i] * b[1];
      v = pt[i] * ppade[i];

      x += (1 & i) ? v : -v;
      out += v;
   }

   pt[0] = x;
   out += x;

   return (out);
}

static double mlsadf2(double x, double *b, const int m, const double a,
                      const int pd, double *d)
{
   double v, out = 0.0, *pt;
   int i;

   pt = &d[pd * (m + 2)];

   for (i = pd; i >= 1; i--) {
      pt[i] = mlsafir(pt[i - 1], b, m, a, &d[(i - 1) * (m + 2)]);
      v = pt[i] * ppade[i];

      x += (1 & i) ? v : -v;
      out += v;
   }

   pt[0] = x;
   out += x;

   return (out);
}

double mlsadf(double x, double *b, const int m, const double a, const int pd,
              double *d)
{
   ppade = &pade[pd * (pd + 1) / 2];

   x = mlsadf1(x, b, a, pd, d);
   x = mlsadf2(x, b, m, a, pd, &d[2 * (pd + 1)]);

   return (x);
}


static double mlsafirt(double x, double *b, const int m, const double a,
                       double *d)
{
   int i;
   double y = 0.0;

   y = (1.0 - a * a) * d[0];

   d[m] = b[m] * x + a * d[m - 1];
   for (i = m - 1; i > 1; i--)
      d[i] += b[i] * x + a * (d[i - 1] - d[i + 1]);
   d[1] += a * (d[0] - d[2]);

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (y);
}

static double mlsadf2t(double x, double *b, const int m, const double a,
                       const int pd, double *d)
{
   double v, out = 0.0, *pt;
   int i;

   pt = &d[pd * (m + 2)];

   for (i = pd; i >= 1; i--) {
      pt[i] = mlsafirt(pt[i - 1], b, m, a, &d[(i - 1) * (m + 2)]);
      v = pt[i] * ppade[i];

      x += (1 & i) ? v : -v;
      out += v;
   }

   pt[0] = x;
   out += x;

   return (out);
}

double mlsadft(double x, double *b, const int m, const double a, const int pd,
               double *d)
{
   ppade = &pade[pd * (pd + 1) / 2];

   x = mlsadf1(x, b, a, pd, d);
   x = mlsadf2t(x, b, m, a, pd, &d[2 * (pd + 1)]);

   return (x);
}
