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

/***********************************************************************

    $Id: _mgc2mgc.c,v 1.18 2016/12/22 10:53:08 fjst15124 Exp $

    Frequency and Generalized Cepstral Transformation

        void mgc2mgc(c1, m1, a1, g1, c2, m2, a2, g2)

        double *c1   : minimum phase sequence (input)
        int     m1   : order of c1
        double  a1   : alpha of c1
        double  g1   : gamma of c1
        double *c2   : transformed sequence (output)
        int     m2   : order of c2
        double  a2   : alpha of c2
        double  g2   : gamma of c2

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

void mgc2mgc(double *c1, const int m1, const double a1, const double g1,
             double *c2, const int m2, const double a2, const double g2)
{
   double a;
   static double *ca = NULL;
   static int size_a;

   if (ca == NULL) {
      ca = dgetmem(m1 + 1);
      size_a = m1;
   }
   if (m1 > size_a) {
      free(ca);
      ca = dgetmem(m1 + 1);
      size_a = m1;
   }

   a = (a2 - a1) / (1 - a1 * a2);

   if (a == 0) {
      movem(c1, ca, sizeof(*c1), m1 + 1);
      gnorm(ca, ca, m1, g1);
      gc2gc(ca, m1, g1, c2, m2, g2);
      ignorm(c2, c2, m2, g2);
   } else {
      freqt(c1, m1, c2, m2, a);
      gnorm(c2, c2, m2, g1);
      gc2gc(c2, m2, g1, c2, m2, g2);
      ignorm(c2, c2, m2, g2);
   }

   return;
}
