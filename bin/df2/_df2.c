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

/******************************************************
 $Id: _df2.c,v 1.18 2016/12/22 10:53:02 fjst15124 Exp $
    2nd order standard form digital filter
        double df2(x,sf,f0p,wbp,f0z,wbz,fp,fz,buf,bufp)

        double   x     : filter input
        double   sf    : sampling frequency
        double   f0p   : center frequency of pole
        double   wbp   : band width of pole
        double   f0z   : center frequency of zero
        double   wbz   : band witdth of zero
        int      fp    : if pole is exist then fp = 1 
        int      fz    : if zero is exist then fz = 1
        double   buf[] : buffer ( which require 3 )
        int      *bufp : work pointer
   
        return         : filter output

                         Dec.1995 Naohiro Isshiki
*******************************************************/

#include <stdio.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

double df2(const double x, const double sf, const double f0p, const double wbp,
           const double f0z, const double wbz, const int fp, const int fz,
           double *buf, int *bufp)
{
   double a[3], b[3];
   double p, e;

   p = 4 * atan(1.0) / sf;
   e = exp(-p * wbz);

   a[0] = 1.0;
   if (fz) {
      a[1] = -2 * e * cos(2 * p * f0z);
      a[2] = e * e;
   } else {
      a[1] = 0;
      a[2] = 0;
   }

   e = exp(-p * wbp);
   b[0] = 1.0;
   if (fp) {
      b[1] = -2 * e * cos(2 * p * f0p);
      b[2] = e * e;
   } else {
      b[1] = 0;
      b[2] = 0;
   }

   return (dfs(x, b, 2, a, 2, buf, bufp));
}
