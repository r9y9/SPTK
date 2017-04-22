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
    $Id: invert.c,v 1.6 2016/12/22 10:53:15 fjst15124 Exp $

    Calculate inverse matrix

    double invert (double **mat, double **inv, int n)

    double  **mat : input matrix
    double  **inv : inverse matrix
    int n         : size of matrix

    return value  : determinant of input matrix

****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

double invert(double **mat, double **inv, const int n)
{
   int i, j, k, *swap, ii, ik;
   double **copy_mat, *tmpmat, d, u, det, *work;

   copy_mat = (double **) malloc(sizeof(double *) * n);
   tmpmat = dgetmem(n * n);

   for (i = 0, j = 0; i < n; i++, j += n) {
      copy_mat[i] = tmpmat + j;
   }
   for (i = 0; i < n; i++) {
      for (k = 0; k < n; k++) {
         copy_mat[i][k] = mat[i][k];
      }
   }

   swap = (int *) malloc(sizeof(int) * n);
   work = dgetmem(n);

   for (k = 0; k < n; k++) {
      swap[k] = k;
      u = 0.0;
      for (j = 0; j < n; j++) {
         d = fabs(copy_mat[k][j]);
         if (d > u) {
            u = d;
         }
      }
      if (u == 0.0) {
         fprintf(stderr, "Can't calculate inverse matrix!\n");
         exit(1);
      }
      work[k] = 1.0 / u;
   }

   det = 1;
   for (k = 0; k < n; k++) {
      u = -1;
      for (i = k; i < n; i++) {
         ii = swap[i];
         d = fabs(copy_mat[ii][k]) * work[ii];
         if (d > u) {
            u = d;
            j = i;
         }
      }

      ik = swap[j];
      if (j != k) {
         swap[j] = swap[k];
         swap[k] = ik;
         det = -det;
      }

      u = copy_mat[ik][k];
      det *= u;
      if (u == 0.0) {
         fprintf(stderr, "Can't calculate inverse matrix!\n");
         exit(1);
      }
      for (i = k + 1; i < n; i++) {
         ii = swap[i];
         d = (copy_mat[ii][k] /= u);
         for (j = k + 1; j < n; j++) {
            copy_mat[ii][j] -= d * copy_mat[ik][j];
         }
      }
   }

   if (det != 0.0) {
      for (k = 0; k < n; k++) {
         for (i = 0; i < n; i++) {
            ii = swap[i];
            d = (ii == k);
            for (j = 0; j < i; j++) {
               d -= copy_mat[ii][j] * inv[j][k];
            }
            inv[i][k] = d;
         }
         for (i = n - 1; i >= 0; i--) {
            d = inv[i][k];
            ii = swap[i];
            for (j = i + 1; j < n; j++) {
               d -= copy_mat[ii][j] * inv[j][k];
            }
            inv[i][k] = d / copy_mat[ii][i];
         }
      }
   } else {
      fprintf(stderr, "Can't calculate inverse matrix!\n");
      exit(1);
   }

   free(copy_mat[0]);
   free(copy_mat);
   free(swap);
   free(work);

   return (det);
}
