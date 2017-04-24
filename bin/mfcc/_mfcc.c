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

    $Id: _mfcc.c,v 1.16 2016/12/22 10:53:07 fjst15124 Exp $

    Mel-Frequency Cepstral Analysis

        void mfcc(in, mc, sampleFreq, alpha, eps, wlng, flng
                  m, n, ceplift, dftmode, usehamming);

        double  *in        : input sequence
        double  *mc        : mel-frequency cepstral coefficients
        double  sampleFreq : sample frequency
        double  alpha      : pre-emphasis coefficient
        double  eps        : epsilon
        int     wlng       : frame length of input sequence
        int     flng       : frame length for fft
        int     m          : order of cepstrum
        int     n          : number of channel for mel-filter bank
        int     ceplift    : liftering coefficients
        Boolean dftmode    : use dft
        Boolean usehamming : use hamming window

******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

#define MEL 1127.01048
#define EZERO (-1.0E10)


double freq_mel(double freq)
{
   return MEL * log(freq / 700.0 + 1.0);
}


double sample_mel(int sample, int num, double fs)
{
   double freq;
   freq = (double) (sample + 1) / (double) (num) * (fs / 2.0);

   return freq_mel(freq);
}

double cal_energy(double *x, const int leng)
{
   int k;
   double energy = 0.0;
   for (k = 0; k < leng; k++)
      energy += x[k] * x[k];

   return ((energy <= 0) ? EZERO : log(energy));
}

void hamming(double *x, const int leng)
{
   int k;
   double arg;

   arg = M_2PI / (leng - 1);
   for (k = 0; k < leng; k++)
      x[k] *= (0.54 - 0.46 * cos(k * arg));
}

void pre_emph(double *x, double *y, const double alpha, const int leng)
{
   int k;
   y[0] = x[0] * (1.0 - alpha);
   for (k = 1; k < leng; k++)
      y[k] = x[k] - x[k - 1] * alpha;
}

void spec(double *x, double *sp, const int leng)
{
   int k, no;
   double *y, *mag;

   no = leng / 2;

   y = dgetmem(leng + no);
   mag = y + leng;

   fftr(x, y, leng);
   for (k = 1; k < no; k++) {
      mag[k] = x[k] * x[k] + y[k] * y[k];
      sp[k] = sqrt(mag[k]);
   }
   free(y);
}

void fbank(double *x, double *fb, const double eps, const double fs,
           const int leng, const int n)
{
   int k, fnum, no, chanNum = 0;
   int *noMel;
   double *w, *countMel;
   double maxMel, kMel;

   no = leng / 2;
   noMel = (int *) getmem((size_t) no, sizeof(int));
   countMel = dgetmem(n + 1 + no);
   w = countMel + n + 1;
   maxMel = freq_mel(fs / 2.0);

   for (k = 0; k <= n; k++)
      countMel[k] = (double) (k + 1) / (double) (n + 1) * maxMel;
   for (k = 1; k < no; k++) {
      kMel = sample_mel(k - 1, no, fs);
      while (countMel[chanNum] < kMel && chanNum <= n)
         chanNum++;
      noMel[k] = chanNum;
   }

   for (k = 1; k < no; k++) {
      chanNum = noMel[k];
      kMel = sample_mel(k - 1, no, fs);
      w[k] = (countMel[chanNum] - kMel) / (countMel[0]);
   }

   for (k = 1; k < no; k++) {
      fnum = noMel[k];
      if (fnum > 0)
         fb[fnum] += x[k] * w[k];
      if (fnum <= n)
         fb[fnum + 1] += (1 - w[k]) * x[k];
   }

   free(noMel);
   free(countMel);

   for (k = 1; k <= n; k++) {
      if (fb[k] < eps)
         fb[k] = eps;
      fb[k] = log(fb[k]);
   }
}



void lifter(double *x, double *y, const int m, const int leng)
{
   int k;
   double theta;
   for (k = 0; k < m; k++) {
      theta = PI * (double) k / (double) leng;
      y[k] = (1.0 + (double) leng / 2.0 * sin(theta)) * x[k];
   }
}

void mfcc(double *in, double *mc, const double sampleFreq, const double alpha,
          const double eps, const int wlng, const int flng, const int m,
          const int n, const int ceplift, const Boolean dftmode,
          const Boolean usehamming)
{
   static double *x = NULL, *px, *wx, *sp, *fb, *dc;
   double energy = 0.0, c0 = 0.0;
   int k;

   if (x == NULL) {
      x = dgetmem(wlng + wlng + flng + flng + n + 1 + m + 1);
      px = x + wlng;
      wx = px + wlng;
      sp = wx + flng;
      fb = sp + flng;
      dc = fb + n + 1;
   } else {
      free(x);
      x = dgetmem(wlng + wlng + flng + flng + n + 1 + m + 1);
      px = x + wlng;
      wx = px + wlng;
      sp = wx + flng;
      fb = sp + flng;
      dc = fb + n + 1;
   }

   movem(in, x, sizeof(*in), wlng);
   /* calculate energy */
   energy = cal_energy(x, wlng);
   pre_emph(x, px, alpha, wlng);
   /* apply hamming window */
   if (usehamming)
      window(HAMMING, px, wlng, 0);
   for (k = 0; k < wlng; k++)
      wx[k] = px[k];
   spec(wx, sp, flng);
   fbank(sp, fb, eps, sampleFreq, flng, n);
   /* calculate 0'th coefficient */
   for (k = 1; k <= n; k++)
      c0 += fb[k];
   c0 *= sqrt(2.0 / (double) n);
   dct(fb + 1, dc, n, m, dftmode, 0);

   /* liftering */
   if (ceplift > 0)
      lifter(dc, mc, m, ceplift);
   else
      movem(dc, mc, sizeof(*dc), m);

   for (k = 0; k < m - 1; k++)
      mc[k] = mc[k + 1];
   mc[m - 1] = c0;
   mc[m] = energy;

}
