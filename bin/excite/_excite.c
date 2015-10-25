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
/*                1996-2014  Nagoya Institute of Technology          */
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
/************************************************************************
*                                                                       *
*    Generate excitation                                                *
*                                                                       *
*                                       1986.6  K.Tokuda                *
*                                       1996.4  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               excite [ options ] [ infile ] > stdout                  *
*       options:                                                        *
*               -p p     :  frame period                        [100]   *
*               -i i     :  interpolation period                [1]     *
*               -n       :  gauss/M-sequence flag for unoiced   [FALSE] *
*                           default is M-sequence                       *
*               -s s     :  seed for nrand                      [1]     *
*      infile:                                                          *
*               pitch data                                              *
*      stdout:                                                          *
*               excitation                                              *
*      require:                                                         *
*               mseq()                                                  *
*                                                                       *
************************************************************************/

/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define FPERIOD 100
#define IPERIOD 1
#define SEED 1
#define GAUSS FA

char *BOOL[] = { "FALSE", "TRUE" };

void excite(double* pitch, int n, double* out, int fprd, int iprd, Boolean gauss, int seed)
{
   int i, j, k = 0;
   int fn;
   unsigned long next = seed;
   double x, p1, p2, inc, pc;

   if (gauss & (seed != 1))
      next = srnd((unsigned int) seed);

   p1 = pitch[0];
   pc = p1;

   for (fn = 1; fn < n; fn++) {
      p2 = pitch[fn];

      if ((p1 != 0.0) && (p2 != 0.0))
         inc = (p2 - p1) * (double) iprd / (double) fprd;
      else {
         inc = 0.0;
         pc = p2;
         p1 = 0.0;
      }

      for (j = fprd, i = (iprd + 1) / 2; j--;) {
         if (p1 == 0.0) {
            if (gauss)
               x = (double) nrandom(&next);
            else
               x = mseq();
         } else {
            if ((pc += 1.0) >= p1) {
               x = sqrt(p1);
               pc = pc - p1;
            } else
               x = 0.0;
         }
         /* k is the current sample index */
         out[k++] = x;

         if (!--i) {
            p1 += inc;
            i = iprd;
         }
      }
      p1 = p2;
   }

   return;
}
