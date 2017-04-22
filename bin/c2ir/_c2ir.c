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

/******************************************************************
 $Id: _c2ir.c,v 1.17 2016/12/22 10:53:01 fjst15124 Exp $

        cepstrum to impulse response
                c2ir(c,nc,h,leng)

                double   *c   : cepstral sequence
                int      nc   : order of cepstrum
                double   *h   : impulse response
                int      leng : length of impulse response

        impulse response to cepstrum
                ic2ir(h,leng,nc,c)

                double   *h   : impulse response
                int      nc   : order of cepstrum
                double   *c   : cepstral sequence
                int      leng : length of impulse response

                         Naohiro Isshiki,   Dec.1995
********************************************************************/

#include <stdlib.h>
#include <math.h>

void c2ir(double *c, const int nc, double *h, const int leng)
{
   int n, k, upl;
   double d;

   h[0] = exp(c[0]);
   for (n = 1; n < leng; n++) {
      d = 0;
      upl = (n >= nc) ? nc - 1 : n;
      for (k = 1; k <= upl; k++)
         d += k * c[k] * h[n - k];
      h[n] = d / n;
   }

   return;
}

void ic2ir(double *h, const int leng, double *c, const int nc)
{
   int n, k, upl;
   double d;

   c[0] = log(h[0]);
   for (n = 1; n < nc; n++) {
      d = (n >= leng) ? 0 : n * h[n];
      upl = (n > leng) ? n - leng + 1 : 1;
      for (k = upl; k < n; k++)
         d -= k * c[k] * h[n - k];
      c[n] = d / (n * h[0]);
   }

   return;
}
