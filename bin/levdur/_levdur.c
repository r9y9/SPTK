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

    $Id: _levdur.c,v 1.23 2016/12/22 10:53:06 fjst15124 Exp $

    Solve an Autocorrelation Normal Equation
    Using Levinson-Durbin Method

       int levdur(r, a, m, eps);

       double  *r    : autocorrelation sequence
       double  *a    : LP coefficients
       int     m     : order of LPC
       double  eps   : singular check (eps(if -1., 1.0e-6 is assumed))

       return  value :  0  -> normally completed
                        -1 -> abnormally completed
                        -2 -> unstable LPC

******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

int levdur(double *r, double *a, const int m, double eps)
{
   int l, k, flag = 0;
   double rmd, mue;
   static double *c = NULL;
   static int size;

   if (c == NULL) {
      c = dgetmem(m + 1);
      size = m;
   }

   if (m > size) {
      free(c);
      c = dgetmem(m + 1);
      size = m;
   }

   if (eps < 0.0)
      eps = 1.0e-6;
   rmd = r[0];
#ifdef WIN32
   if ((((rmd < 0.0) ? -rmd : rmd) <= eps) || _isnan(rmd))
      return (-1);
#else
   if ((((rmd < 0.0) ? -rmd : rmd) <= eps) || isnan(rmd))
      return (-1);
#endif
   a[0] = 0.0;

   for (l = 1; l <= m; l++) {
      mue = -r[l];
      for (k = 1; k < l; k++)
         mue -= c[k] * r[l - k];
      mue = mue / rmd;

      for (k = 1; k < l; k++)
         a[k] = c[k] + mue * c[l - k];
      a[l] = mue;

      rmd = (1.0 - mue * mue) * rmd;
#ifdef WIN32
      if ((((rmd < 0.0) ? -rmd : rmd) <= eps) || _isnan(rmd))
         return (-1);
#else
      if ((((rmd < 0.0) ? -rmd : rmd) <= eps) || isnan(rmd))
         return (-1);
#endif
      if (((mue < 0.0) ? -mue : mue) >= 1.0)
         flag = -2;

      for (k = 0; k <= l; k++)
         c[k] = a[k];
   }
   a[0] = sqrt(rmd);

   return (flag);
}
