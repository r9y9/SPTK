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

/*****************************************************************

    $Id: matrix.c,v 1.18 2016/12/22 10:53:15 fjst15124 Exp $

    compute matrix functions

    multiple matrix
    multim(x, xx, xy, y, yx, yy, a)

        double x[] : matrix1
        int     xx : row number of matrix1
        int     xy : line number of matrix1
        double y[] : matrix2
        int     yx : row number of matrix2
        int     yy : line number of matrix2
        double a[] : answer

        if xx=1 and xy=1 ,then x[0] is regarded as scalar
		
    add matrix
    addm(x, y, xx, yy, a)
        double x[] : matrix1
        double y[] : matrix2
        int     xx : row number 
        int     xy : line number
        double a[] : answer

                        Naohiro Isshiki Feb.1996
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

static double *tmp;
static int tmpsize = 0;

static void mm(double x[], const int xx, const int xy, double y[], const int yx,
               const int yy, double a[])
{
   int i, j, k;
   double *wx, *wy;

   if (xx == 1 && xy == 1) {
      for (i = yx * yy - 1; i >= 0; i--)
         a[i] = x[0] * y[i];
      return;
   }

   if (xx != yy) {
      fprintf(stderr, "Invalid matrix size x= %d*%d,y= %d*%d\n", xx, xy, yx,
              yy);
      exit(1);
   }

   wx = x;
   for (i = 0; i < xy; i++) {
      for (j = 0; j < yx; j++) {
         wy = &y[j];
         *a = 0;
         for (k = 0; k < xx; k++) {
            *a += *wx * *wy;
            wx++;
            wy += yx;
         }
         wx -= xx;
         a++;
      }
      wx += xx;
   }

   return;
}

void multim(double x[], const int xx, const int xy, double y[], const int yx,
            const int yy, double a[])
{
   int i;

   if (x == a) {
      if (((xy > yy) ? xy : yy) * yx > tmpsize) {
         if (tmp != NULL)
            free(tmp);
         tmpsize = ((xy > yy) ? xy : yy) * yx;
         tmp = (double *) getmem(tmpsize, sizeof(*tmp));
      }
      mm(x, xx, xy, y, yx, yy, tmp);
      if (xx == xy)
         for (i = yx * yy - 1; i >= 0; i--)
            a[i] = tmp[i];
      else
         for (i = xy * yx - 1; i >= 0; i--)
            a[i] = tmp[i];
   } else {
      mm(x, xx, xy, y, yx, yy, a);
   }

   return;
}

static void am(double x[], double y[], const int xx, const int yy, double a[])
{
   int i, j;

   for (i = 0; i < yy; i++)
      for (j = 0; j < xx; j++)
         a[j + i * xx] = x[j + i * xx] + y[j + i * xx];
}

void addm(double x[], double y[], const int xx, const int yy, double a[])
{
   int i;

   if (x == a) {
      if (xx * yy > tmpsize) {
         if (tmp != NULL)
            free(tmp);
         tmpsize = xx * yy;
         tmp = (double *) getmem(tmpsize, sizeof(*tmp));
      }
      am(x, y, xx, yy, tmp);
      for (i = xx * yy - 1; i >= 0; i--)
         a[i] = tmp[i];
   } else {
      am(x, y, xx, yy, a);
   }

   return;
}
