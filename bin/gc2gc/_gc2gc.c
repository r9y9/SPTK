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
    $Id: _gc2gc.c,v 1.19 2016/12/22 10:53:04 fjst15124 Exp $

    Generalized Cepstral Transformation   

        void gc2gc(c1, m1, g1, c2, m2, g2)

        double   *c1   : normalized generalized cepstrum (input)
        int      m1    : order of gc1
        double   g1    : gamma of gc1
        double   *c2   : normalized generalized cepstrum (output)
        int      m2    : order of gc2
        double   g2    : gamma of gc2

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

void gc2gc(double *c1, const int m1, const double g1, double *c2, const int m2,
           const double g2)
{
   int i, min, k, mk;
   double ss1, ss2, cc;
   static double *ca = NULL;
   static int size;

   if (ca == NULL) {
      ca = dgetmem(m1 + 1);
      size = m1;
   }
   if (m1 > size) {
      free(ca);
      ca = dgetmem(m1 + 1);
      size = m1;
   }

   movem(c1, ca, sizeof(*c1), m1 + 1);

   c2[0] = ca[0];
   for (i = 1; i <= m2; i++) {
      ss1 = ss2 = 0.0;
      min = (m1 < i) ? m1 : i - 1;
      for (k = 1; k <= min; k++) {
         mk = i - k;
         cc = ca[k] * c2[mk];
         ss2 += k * cc;
         ss1 += mk * cc;
      }

      if (i <= m1)
         c2[i] = ca[i] + (g2 * ss2 - g1 * ss1) / i;
      else
         c2[i] = (g2 * ss2 - g1 * ss1) / i;
   }

   return;
}
