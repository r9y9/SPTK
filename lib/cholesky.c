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
    $Id: cholesky.c,v 1.16 2016/12/22 10:53:15 fjst15124 Exp $

    Solve Linear Set of Equations
		       Using Cholesky Decomposition 

    int cholesky(c, a, b, n, eps) < Ca=b >

    double *c  : symmetrical coefficient matrix
    double *a  : solution vector
    double *b  : constant vector
    int     n  : order of vector
    double eps : error check (if -1.0, 1.0e-6 is assumed)

    return value : 0 -> normally completed
                  -1 -> abnormally completed

****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

int cholesky(double *c, double *a, double *b, const int n, double eps)
{
   int i, j, k;
   static double *d = NULL, *y, *v, *vp;
   static int size;

   if (d == NULL) {
      d = dgetmem(n * (n + 2));
      y = d + n;
      v = y + n;
      size = n;
   }

   if (n > size) {
      free(d);
      d = dgetmem(n * (n + 2));
      y = d + n;
      v = y + n;
      size = n;
   }

   if (eps < 0.0)
      eps = 1.0e-6;

   for (j = 0; j < n; j++, c += n) {
      d[j] = c[j];
      vp = v + j * n;
      for (k = 0; k < j; k++)
         d[j] -= vp[k] * vp[k] * d[k];

      if (fabs(d[j]) <= eps)
         return (-1);

      for (i = j + 1; i < n; i++) {
         vp = v + i * n;
         vp[j] = c[i];
         for (k = 0; k < j; k++)
            vp[j] -= vp[k] * v[j * n + k] * d[k];
         vp[j] /= d[j];
      }
   }

   for (i = 0; i < n; i++) {
      y[i] = b[i];
      vp = v + i * n;
      for (k = 0; k < i; k++)
         y[i] -= vp[k] * y[k];
   }

   for (i = n - 1; i >= 0; i--) {
      a[i] = y[i] / d[i];
      for (k = i + 1; k < n; k++)
         a[i] -= v[n * k + i] * a[k];
   }
   return (0);
}
