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

    $Id: _zerodf.c,v 1.18 2016/12/22 10:53:15 fjst15124 Exp $

    All Zero Digital Filter

       double zerodf(x, b, m, d)

       double   x     : input 
       double  *b     : MA coefficients 
       int      m     : order of coefficients
       double  *d     : delay

       return   value : filtered data

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

double zerodf(double x, double *b, int m, double *d)
{
   double out;

   out = b[0] * x;

   for (m--; m > 0; m--) {
      out += b[m + 1] * d[m];
      d[m] = d[m - 1];
   }
   out += b[1] * d[0];
   d[0] = x;

   return (out);
}

double zerodft(double x, double *b, const int m, double *d)
{
   int i;
   double out;

   out = b[0] * x + d[0];

   for (i = 1; i < m; i++)
      d[i - 1] = b[i] * x + d[i];

   d[m - 1] = b[m] * x;

   return (out);
}

double zerodf1(double x, double *b, int m, double *d)
{
   double out;

   out = x;
   for (m--; m > 0; m--) {
      out += b[m + 1] * d[m];
      d[m] = d[m - 1];
   }
   out += b[1] * d[0];
   d[0] = x;

   return (out);
}

double zerodf1t(double x, double *b, const int m, double *d)
{
   int i;
   double out;

   out = x + d[0];

   for (i = 1; i < m; i++)
      d[i - 1] = b[i] * x + d[i];

   d[m - 1] = b[m] * x;

   return (out);
}
