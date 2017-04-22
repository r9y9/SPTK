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

/**********************************************************************************************

    $Id: _lbg.c,v 1.23 2016/12/22 10:53:06 fjst15124 Exp $

    LBG Algorithm for Vector Quantizer Design

       void lbg(x, l, tnum, icb, icbsize, cb, ecbsize, iter, mintnum, seed, centup, delta, end)

       double *x      :   training vector
       double l       :   length of vector
       int    tnum    :   number of training vector
       double *icb    :   initial codebook
       int    icbsize :   initial codebook size
       double *cb     :   final codebook
       int    ecbsize :   final codebook size
       int    iter    :   maximum number of iteration for centroid update
       int    mintnum :   minimum number of training vectors for each cell
       int    seed    :   seed for normalized random vector
       int    centup  :   type of exception procedure for centroid update
       double delta   :   splitting factor
       double end     :   end condition

***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

#define MAXVALUE 1e23
#define SEED 1
#define abs(x) ( (x<0) ? (-(x)) : (x) )

void lbg(double *x, const int l, const int tnum, double *icb, int icbsize,
         double *cb, const int ecbsize, const int iter, const int mintnum,
         const int seed, const int centup, const double delta, const double end)
{
   int i, j, k, it, maxindex, tnum1, tnum2;
   static int *cntcb, *tindex, size, sizex, sizecb;
   unsigned long next = SEED;
   double d0, d1, dl, err, tmp, rand;
   static double *cb1 = NULL;
   double *p, *q, *r;

   if (cb1 == NULL) {
      cb1 = dgetmem(ecbsize * l);
      tindex = (int *) dgetmem(tnum);
      cntcb = (int *) dgetmem(ecbsize);
      size = l;
      sizex = tnum;
      sizecb = ecbsize;
   }
   if (l > size) {
      free(cb1);
      cb1 = dgetmem(ecbsize * l);
      size = l;
   }
   if (tnum > sizex) {
      free(tindex);
      tindex = (int *) dgetmem(tnum);
      sizex = tnum;
   }
   if (sizecb > ecbsize) {
      free(cb1);
      free(cntcb);
      cb1 = dgetmem(ecbsize * l);
      cntcb = (int *) dgetmem(ecbsize);
   }

   movem(icb, cb, sizeof(*icb), icbsize * l);

   if (seed != 1)
      next = srnd((unsigned int) seed);

   for (; icbsize * 2 <= ecbsize;) {
      q = cb;
      r = cb + icbsize * l;
      for (i = 0; i < icbsize; i++) {
         for (j = 0; j < l; j++) {
            dl = delta * nrandom(&next);
            *r = *q - dl;
            r++;
            *q = *q + dl;
            q++;
         }
      }
      icbsize *= 2;

      d0 = MAXVALUE;
      for (it = 1; it <= iter; it++) {
         fillz((double *) cntcb, sizeof(*cntcb), icbsize);
         d1 = 0.0;
         p = x;
         for (i = 0; i < tnum; i++, p += l) {
            tindex[i] = vq(p, cb, l, icbsize);
            cntcb[tindex[i]]++;

            q = cb + tindex[i] * l;
            d1 += edist(p, q, l);
         }


         d1 /= tnum;
         err = abs((d0 - d1) / d1);

         if (err < end)
            break;

         d0 = d1;
         fillz(cb1, sizeof(*cb), icbsize * l);

         p = x;
         for (i = 0; i < tnum; i++) {
            q = cb1 + tindex[i] * l;
            for (j = 0; j < l; j++)
               *q++ += *p++;
         }

         k = maxindex = 0;
         for (i = 0; i < icbsize; i++)
            if (cntcb[i] > k) {
               k = cntcb[i];
               maxindex = i;
            }


         q = cb;
         r = cb1;
         for (i = 0; i < icbsize; i++, r += l, q += l)
            if (cntcb[i] >= mintnum)
               for (j = 0; j < l; j++)
                  q[j] = r[j] / (double) cntcb[i];
            else {
               if (centup == 1) {
                  p = cb + maxindex * l;
                  for (j = 0; j < l; j++) {
                     rand = nrandom(&next);
                     q[j] = p[j] + delta * rand;
                     p[j] = p[j] - delta * rand;
                  }
               } else if (centup == 2) {
                  if (i < icbsize / 2) {
                     p = q + icbsize / 2 * l;
                     tnum1 = cntcb[i];
                     tnum2 = cntcb[i + icbsize / 2];
                     for (j = 0; j < l; j++) {
                        tmp = (tnum2 * q[j] + tnum1 * p[j]) / (tnum1 + tnum2);
                        rand = nrandom(&next);
                        q[j] = tmp + delta * rand;
                        p[j] = tmp - delta * rand;
                     }
                  } else {
                     p = q - icbsize / 2 * l;
                     tnum1 = cntcb[i];
                     tnum2 = cntcb[i - icbsize / 2];
                     for (j = 0; j < l; j++) {
                        tmp = (tnum2 * q[j] + tnum1 * p[j]) / (tnum1 + tnum2);
                        rand = nrandom(&next);
                        q[j] = tmp + delta * rand;
                        p[j] = tmp - delta * rand;
                     }
                  }
               }
            }
      }
      if (icbsize == ecbsize)
         break;
   }

   return;
}
