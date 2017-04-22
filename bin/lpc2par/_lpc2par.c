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

/***************************************************************

    $Id: _lpc2par.c,v 1.18 2016/12/22 10:53:06 fjst15124 Exp $

    Transformation LPC to PARCOR

        int lpc2par(a, k, m)

        double  *a : LP coefficients
        double  *k : PARCOR coefficients
        int      m : order of LPC

        return value :  0  -> normally completed
                        -1 -> abnormally completed

****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

int lpc2par(double *a, double *k, const int m)
{
   int i, n, flg = 0;
   double s;
   static double *kk = NULL, *aa;
   static int size;

   if (kk == NULL) {
      kk = dgetmem(m + m + 2);
      aa = kk + m + 1;
      size = m;
   }

   if (m > size) {
      free(kk);
      kk = dgetmem(m + m + 2);
      aa = kk + m + 1;
      size = m;
   }

   movem(a, aa, sizeof(*aa), m + 1);

   kk[0] = aa[0];
   for (n = m; n >= 1; n--) {
      movem(&aa[1], &kk[1], sizeof(*aa), n);

      if (kk[n] >= 1.0 || kk[n] <= -1.0)
         flg = -1;

      s = 1.0 - kk[n] * kk[n];
      for (i = 1; i < n; i++)
         aa[i] = (kk[i] - kk[n] * kk[n - i]) / s;
   }
   movem(kk, k, sizeof(*kk), m + 1);

   return (flg);
}
