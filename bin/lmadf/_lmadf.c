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

    LMA Digital Filter

        double    lmadflt(x, c, m, pd, d)

        double    x     : input
        double    *c    : cepstrum
        int       m     : order of cepstrum
        int       pd    : order of Pade approximation
        double    *d    : delay

        return    value : filtered data

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>

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

/****************************************************************

    double lmafir(x, c, d, m, m1, m2)

    double x  : input
    double *c : cepstrum
    int    m  : order of cepstrum
    double *d : delay
    int    m1 : start order
    int    m2 : end order

*****************************************************************/

static double lmafir(double x, double *c, const int m, double *d, const int m1,
                     const int m2)
{
   int i;

   for (i = m - 1; i >= 1; i--)
      d[i] = d[i - 1];
   d[0] = x;
   for (x = 0.0, i = m1; i <= m2; i++)
      x += c[i] * d[i - 1];

   return (x);
}

double lmadf(double x, double *c, const int m, const int pd, double *d)
{
   ppade = &pade[pd * (pd + 1) / 2];

   x = lmadf1(x, c, m, d, pd, 1, 1);    /* D1(z) */
   x = lmadf1(x, c, m, &d[(m + 1) * pd], pd, 2, m);     /* D2(z) */

   return (x);
}

double cascade_lmadf(double x, double *c, const int m, const int pd, double *d,
                     const int block_num, int *block_size)
{
   int i, block_start = 1, block_end = 0;
   ppade = &pade[pd * (pd + 1) / 2];

   for (i = 0; i < block_num; i++) {
      block_end += abs(block_size[i]);
      if (block_size[i] > 0) {
         x = lmadf1(x, c, m, d, pd, block_start, block_end);
      }
      d += (m + 1) * pd;
      block_start += abs(block_size[i]);
   }

   return (x);
}

/****************************************************************

    double lmadf1(x, c, m, d, m1, m2, pd)

    double x  : input
    double *c : cepstrum
    int    m  : order of cepstrum
    double *d : delay
    int    m1 : start order
    int    m2 : end order
    int    pd : order of Pade approximation

*****************************************************************/

double lmadf1(double x, double *c, const int m, double *d, const int pd,
              const int m1, const int m2)
{
   double y, t, *pt;
   int i;

   pt = &d[pd * m];
   t = lmafir(pt[pd - 1], c, m, &d[(pd - 1) * m], m1, m2);
   y = (t *= ppade[pd]);
   x += (1 & pd) ? t : -t;
   for (i = pd - 1; i >= 1; i--) {
      pt[i] = t = lmafir(pt[i - 1], c, m, &d[(i - 1) * m], m1, m2);
      y += (t *= ppade[i]);
      x += (1 & i) ? t : -t;
   }
   y += (pt[0] = x);

   return (y);
}

double lmadf1t(double x, double *b, const int pd, double *d)
{
   double v, out = 0.0, *pt;
   int i;

   pt = &d[pd + 1];

   for (i = pd; i >= 1; i--) {
      d[i] = pt[i - 1];
      pt[i] = d[i] * b[1];
      v = pt[i] * ppade[i];

      x += (1 & i) ? v : -v;
      out += v;
   }

   pt[0] = x;
   out += x;

   return (out);
}

static double lmafirt(double x, double *b, const int m, double *d, const int m1,
                      const int m2)
{
   int i;
   double y = 0.0;

   y = d[1];

   d[m2] = b[m2] * x;
   for (i = m2 - 1; i >= m1; i--)
      d[i] += b[i] * x;
   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (y);
}

double lmadf2t(double x, double *b, const int m, const int pd, double *d,
               const int m1, const int m2)
{
   double v, out = 0.0, *pt;
   int i;

   pt = &d[pd * (m + 1)];

   for (i = pd; i >= 1; i--) {
      pt[i] = lmafirt(pt[i - 1], b, m, &d[(i - 1) * (m + 1)], m1, m2);
      v = pt[i] * ppade[i];

      x += (1 & i) ? v : -v;
      out += v;
   }

   pt[0] = x;
   out += x;

   return (out);
}

double lmadft(double x, double *c, const int m, const int pd, double *d,
              int block_num, int *block_size)
{
   int i, block_start = 2, block_end = 1;
   ppade = &pade[pd * (pd + 1) / 2];

   x = lmadf1t(x, c, pd, d);

   for (i = 1; i < block_num; i++) {
      block_end += abs(block_size[i]);
      if (block_size[i] > 0) {
         x = lmadf2t(x, c, m, pd, &d[2 * (pd + 1)], block_start, block_end);
      }
      d += pd * (m + 1) + (pd + 1);
      block_start += abs(block_size[i]);
   }

   return (x);
}
