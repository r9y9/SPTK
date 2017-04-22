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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

/* workspace */
static int dct_table_size = 0;
static double *dct_workspace = NULL;
static double *pLocalReal = NULL;
static double *pLocalImag = NULL;
static double *pWeightReal = NULL;
static double *pWeightImag = NULL;

static int dct_table_size_fft = 0;
static double *dct_workspace2 = NULL;
static double *pLocalReal2 = NULL;
static double *pLocalImag2 = NULL;
static double *pWeightReal2 = NULL;
static double *pWeightImag2 = NULL;


int dft(double *pReal, double *pImag, const int nDFTLength)
{
   int k, n;
   double *pTempReal, *pTempImag, TempReal, TempImag;

   pTempReal = dgetmem(nDFTLength);
   pTempImag = dgetmem(nDFTLength);

   memcpy(pTempReal, pReal, sizeof(double) * nDFTLength);
   memcpy(pTempImag, pImag, sizeof(double) * nDFTLength);

   for (k = 0; k < nDFTLength; k++) {
      TempReal = 0;
      TempImag = 0;
      for (n = 0; n < nDFTLength; n++) {
         TempReal += pTempReal[n] * cos(2.0 * PI * n * k / (double) nDFTLength)
             + pTempImag[n] * sin(2.0 * PI * n * k / (double) nDFTLength);
         TempImag += -pTempReal[n] * sin(2.0 * PI * n * k / (double) nDFTLength)
             + pTempImag[n] * cos(2.0 * PI * n * k / (double) nDFTLength);
      }
      pReal[k] = TempReal;
      pImag[k] = TempImag;
   }
   free(pTempReal);
   free(pTempImag);

   return (0);
}

int dct_create_table_fft(const int nSize)
{
   register int k;

   if (nSize == dct_table_size_fft) {
      /* no needs to resize workspace. */
      return (0);
   } else {
      /* release resources to resize workspace. */
      if (dct_workspace2 != NULL) {
         free(dct_workspace2);
         dct_workspace2 = NULL;
      }
      pLocalReal2 = NULL;
      pLocalImag2 = NULL;
      pWeightReal2 = NULL;
      pWeightImag2 = NULL;
   }

   /* getting resources. */
   if (nSize <= 0) {
      dct_table_size_fft = 0;
      return (0);
   } else {
      dct_table_size_fft = nSize;
      dct_workspace2 = dgetmem(dct_table_size_fft * 6);
      pWeightReal2 = dct_workspace2;
      pWeightImag2 = dct_workspace2 + dct_table_size_fft;
      pLocalReal2 = dct_workspace2 + (2 * dct_table_size_fft);
      pLocalImag2 = dct_workspace2 + (4 * dct_table_size_fft);

      for (k = 0; k < dct_table_size_fft; k++) {
         pWeightReal2[k] =
             cos(k * PI / (2.0 * dct_table_size_fft)) /
             sqrt(2.0 * dct_table_size_fft);
         pWeightImag2[k] =
             -sin(k * PI / (2.0 * dct_table_size_fft)) /
             sqrt(2.0 * dct_table_size_fft);
      }
      pWeightReal2[0] /= sqrt(2.0);
      pWeightImag2[0] /= sqrt(2.0);
   }

   return (0);
}

int dct_create_table(const int nSize)
{
   register int k;

   if (nSize == dct_table_size) {
      /* no needs to resize workspace. */
      return (0);
   } else {
      /* release resources to resize workspace. */
      if (dct_workspace != NULL) {
         free(dct_workspace);
         dct_workspace = NULL;
      }
      pLocalReal = NULL;
      pLocalImag = NULL;
      pWeightReal = NULL;
      pWeightImag = NULL;
   }

   /* getting resources. */
   if (nSize <= 0) {
      dct_table_size = 0;
      return (0);
   } else {
      dct_table_size = nSize;
      dct_workspace = dgetmem(dct_table_size * 6);
      pWeightReal = dct_workspace;
      pWeightImag = dct_workspace + dct_table_size;
      pLocalReal = dct_workspace + (2 * dct_table_size);
      pLocalImag = dct_workspace + (4 * dct_table_size);

      for (k = 0; k < dct_table_size; k++) {
         pWeightReal[k] =
             cos(k * PI / (2.0 * dct_table_size)) / sqrt(2.0 * dct_table_size);
         pWeightImag[k] =
             -sin(k * PI / (2.0 * dct_table_size)) / sqrt(2.0 * dct_table_size);
      }
      pWeightReal[0] /= sqrt(2.0);
      pWeightImag[0] /= sqrt(2.0);
   }

   return (0);
}

int dct_based_on_fft(double *pReal, double *pImag, const double *pInReal,
                     const double *pInImag)
{
   register int n, k;


   for (n = 0; n < dct_table_size_fft; n++) {
      pLocalReal2[n] = pInReal[n];
      pLocalImag2[n] = pInImag[n];
      pLocalReal2[dct_table_size_fft + n] = pInReal[dct_table_size_fft - 1 - n];
      pLocalImag2[dct_table_size_fft + n] = pInImag[dct_table_size_fft - 1 - n];
   }


   fft(pLocalReal2, pLocalImag2, dct_table_size_fft * 2);       /* double input */


   for (k = 0; k < dct_table_size_fft; k++) {
      pReal[k] =
          pLocalReal2[k] * pWeightReal2[k] - pLocalImag2[k] * pWeightImag2[k];
      pImag[k] =
          pLocalReal2[k] * pWeightImag2[k] + pLocalImag2[k] * pWeightReal2[k];
   }

   return (0);
}

int dct_based_on_dft(double *pReal, double *pImag, const double *pInReal,
                     const double *pInImag)
{
   register int n, k;

   for (n = 0; n < dct_table_size; n++) {
      pLocalReal[n] = pInReal[n];
      pLocalImag[n] = pInImag[n];
      pLocalReal[dct_table_size + n] = pInReal[dct_table_size - 1 - n];
      pLocalImag[dct_table_size + n] = pInImag[dct_table_size - 1 - n];
   }

   dft(pLocalReal, pLocalImag, dct_table_size * 2);

   for (k = 0; k < dct_table_size; k++) {
      pReal[k] =
          pLocalReal[k] * pWeightReal[k] - pLocalImag[k] * pWeightImag[k];
      pImag[k] =
          pLocalReal[k] * pWeightImag[k] + pLocalImag[k] * pWeightReal[k];
   }

   return (0);
}

void dct(double *in, double *out, const int size, const int m,
         const Boolean dftmode, const Boolean compmode)
{
   int k, i, j, iter;
   double *pReal, *pImag;
   double *x, *y;
   double *x2, *y2;

   x = dgetmem(2 * size);
   y = x + size;
   pReal = dgetmem(2 * size);
   pImag = pReal + size;
   x2 = dgetmem(2 * size);
   y2 = x2 + size;

   for (k = 0; k < size; k++) {
      x[k] = in[k];
      y[k] = in[k + size];
      x2[k] = x[k];
      y2[k] = y[k];
   }

   iter = 0;
   i = size;
   while ((i /= 2) != 0) {
      iter++;
   }
   j = 1;
   for (i = 1; i <= iter; i++) {
      j *= 2;
   }
   if (size != j || dftmode) {
      dct_create_table(size);
      dct_based_on_dft(pReal, pImag, x2, y2);
   } else {
      dct_create_table_fft(size);
      dct_based_on_fft(pReal, pImag, x2, y2);
   }

   for (k = 0; k < m; k++) {
      out[k] = pReal[k];
      if (compmode == TR) {
         out[k + size] = pImag[k];
      }
   }

   free(x);
   free(x2);
   free(pReal);
}
