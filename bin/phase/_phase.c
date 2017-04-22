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

/*******************************************************************
  $Id: _phase.c,v 1.18 2016/12/22 10:53:10 fjst15124 Exp $
  comupte phase of digital filter or real seaquence
     phase(p, mp, z, mz, ph, flng)

     double *p    : numerator cofficients
     int     mp   : order of numerator polynomial
     double *z    : denominator cofficients
     int     mz   : order of denominator polynomilal
     doble *ph    : phase
     int    flng  : FFT size
     int    unlap : unlapping

                                       Naohiro Isshiki  Feb.1995
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

void phase(double *p, const int mp, double *z, const int mz, double *ph,
           const int flng, const int unlap)
{
   static double *x;
   static int fsize = 0;
   double *y, *xx, *yy, *py;
   int no, i, offset;
   double pi;

   pi = atan(1.) * 4.;

   no = flng / 2 + 1;

   if (flng > fsize) {
      if (x != NULL)
         free(x);
      fsize = flng;
      x = dgetmem(4 * flng + no);
   }
   y = &x[flng];
   xx = &y[flng];
   yy = &xx[flng];
   py = &yy[flng];

   fillz(x, sizeof(*x), flng);
   fillz(xx, sizeof(*xx), flng);
   movem(z, x, mz + 1, sizeof(*z));
   movem(p, xx, mp + 1, sizeof(*p));

   fftr(x, y, flng);
   xx[0] = 1;
   fftr(xx, yy, flng);
   for (i = 0; i < no; i++) {
      ph[i] = x[i] * xx[i] + y[i] * yy[i];
      py[i] = y[i] * xx[i] - x[i] * yy[i];
   }
   offset = 0;
   i = 0;
   ph[i] = atan2(py[i], ph[i]) / pi;
   i++;
   for (; i < no; i++) {
      ph[i] = atan2(py[i], ph[i]) / pi;
      if (unlap) {
         if (ph[i - 1] - ph[i] - offset > 1)
            offset += 2;
         else if (ph[i] + offset - ph[i - 1] > 1)
            offset -= 2;
         ph[i] += offset;
      }
   }

   return;
}
