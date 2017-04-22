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

    $Id: _fftcep.c,v 1.20 2016/12/22 10:53:03 fjst15124 Exp $

    FFT Cepstral Analysis

        void fftcep(sp, flng, c, m, itr, ac);

        double *sp  : log power spectrum
        int    flng : frame length
        double *c   : cepstral coefficients
        int    m    : order of cepstrum
        int    itr  : number of iteration
        double ac   : accelation factor

******************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

void fftcep(double *sp, const int flng, double *c, const int m, int itr,
            double ac)
{
   double temp;
   static double *x = NULL, *y;
   static int size;
   int k;

   if (x == NULL) {
      x = dgetmem(flng + flng);
      y = x + flng;
   }
   if (flng > size) {
      free(x);
      x = dgetmem(flng + flng);
      y = x + flng;
      size = flng;
   }

   movem(sp, x, sizeof(*sp), flng);

   fftr(x, y, flng);
   for (k = 0; k < flng; k++)
      x[k] /= flng;
   for (k = 0; k <= m; k++) {
      c[k] = x[k];
      x[k] = 0;
   }

   ac += 1.0;
   while (--itr > 0) {
      for (k = 1; k <= m; k++)
         x[flng - k] = x[k];

      fftr(x, y, flng);

      for (k = 0; k < flng; k++)
         if (x[k] < 0.0)
            x[k] = 0.0;
         else
            x[k] /= flng;

      fftr(x, y, flng);

      for (k = 0; k <= m; k++) {
         temp = x[k] * ac;
         c[k] += temp;
         x[k] -= temp;
      }
   }
   c[0] *= 0.5;

   if (m == flng / 2)
      c[m] *= 0.5;
}
