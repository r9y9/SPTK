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

    $Id: _lspdf.c,v 1.20 2016/12/22 10:53:07 fjst15124 Exp $

    LSP Speech Synthesis Digital Filter

        double lspdf_even(x, a, m, d)

        double   x     : input
        double  *f     : LSP coefficients
        int      m     : order of coefficients
        double  *d     : delay

        return   value : filtered data

*****************************************************************/

#include <stdio.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

double lspdf_even(double x, double *f, const int m, double *d)
{
   double *d1, *d2, *lsp, x1, x2;
   int i;

   d1 = d + 1;
   d2 = d1 + m;
   lsp = f + 1;
   x1 = x2 = d[0];

   for (i = 0; i < m; i += 2) {
      d1[i] -= 2.0 * x1 * cos(lsp[i]);
      d2[i] -= 2.0 * x2 * cos(lsp[i + 1]);
      d1[i + 1] += x1;
      d2[i + 1] += x2;
      x += d1[i] + d2[i];
      x1 = d1[i + 1];
      x2 = d2[i + 1];
   }

   x -= d2[m - 1] - d1[m - 1];

   for (i = m - 1; i > 0; i--) {
      d1[i] = d1[i - 1];
      d2[i] = d2[i - 1];
   }
   d1[0] = d2[0] = d[0];
   d[0] = -0.5 * x;

   return (x);
}

double lspdf_odd(double x, double *f, const int m, double *d)
{
   int i;
   int mh1;
   double *d1, *d2, *lsp, x1, x2;

   mh1 = (m + 1) / 2;

   d1 = d + 1;
   d2 = d1 + (mh1 + mh1 - 1);
   lsp = f + 1;
   x1 = x2 = d[0];

   for (i = 0; i < m - 1; i += 2) {
      d1[i] -= 2.0 * x1 * cos(lsp[i]);
      d2[i] -= 2.0 * x2 * cos(lsp[i + 1]);
      d1[i + 1] += x1;
      d2[i + 1] += x2;
      x += d1[i] + d2[i];
      x1 = d1[i + 1];
      x2 = d2[i + 1];
   }
   d1[i] -= 2.0 * x1 * cos(lsp[i]);
   x += d1[i] - d2[i];

   for (i = m - 1; i > 0; i--) {
      d1[i] = d1[i - 1];
      d2[i] = d2[i - 1];
   }
   d1[0] = d2[0] = d[0];
   d[0] = -0.5 * x;

   return (x);
}
