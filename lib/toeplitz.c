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
    $Id: toeplitz.c,v 1.16 2016/12/22 10:53:16 fjst15124 Exp $

    Solve a Synmetric Toeplitz Set of Linear Equations	

    int toeplitz(t, a, b, n, eps)   < Ta=b >

    double  *t : Toeplitz elements -> T(i,j) = t(|i-j|) t[0]..t[n-1]
    double  *a : solution vector of equation
    double  *b : known vector
    int      n : system order
    double eps : singular check (eps(if -1., 1.0e-6 is assumed))

    return value : 0  -> normally completed
                   -1 -> abnormally completed

****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

int toeplitz(double *t, double *a, double *b, const int n, double eps)
{
   int l, k;
   static double *c = NULL, *cc;
   static int size;
   double rmd, mue, mue2;

   if (c == NULL) {
      c = dgetmem(n + n + 2);
      cc = c + n;
      size = n;
   }
   if (n > size) {
      free(c);
      c = dgetmem(n + n + 2);
      cc = c + n;
      size = n;
   }

   if (eps < 0.0)
      eps = 1.0e-6;

   fillz(c, sizeof(*c), n + 1);

   rmd = t[0];
   if (((rmd < 0.0) ? -rmd : rmd) <= eps)
      return (-1);

   a[0] = b[0] / rmd;

   for (l = 1; l < n; l++) {
      mue = -t[l];
      for (k = 1; k < l; k++)
         mue -= c[k] * t[l - k];
      mue /= rmd;

      for (k = 1; k < l; k++)
         cc[k] = c[k] + mue * c[l - k];
      cc[l] = mue;

      rmd = (1.0 - mue * mue) * rmd;
      if (((rmd < 0.0) ? -rmd : rmd) <= eps)
         return (-1);

      for (k = 1; k <= l; k++)
         c[k] = cc[k];

      mue2 = b[l];
      for (k = 0; k <= l - 1; k++)
         mue2 += c[l - k] * b[k];
      mue2 /= rmd;

      for (k = 0; k < l; k++)
         a[k] += mue2 * c[l - k];
      a[l] = mue2;
   }

   return (0);
}
