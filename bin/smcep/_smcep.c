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

    $Id: _smcep.c,v 1.30 2016/12/22 10:53:11 fjst15124 Exp $

    Mel-Cepstral Analysis (2nd order all-pass filter)

        int smcep(xw, flng, mc, m, fftsz, a, t, itr1, itr2, dd, 
                  etype, e, f, itype);

        double   *xw   : input sequence
        int      flng  : frame length
        double   *mc   : mel cepstrum
        int      m     : order of mel cepstrum
        double   a     : alpha
        double   t     : theta
        int      itr1  : minimum number of iteration
        int      itr2  : maximum number of iteration
        double   dd    : end condition
        int      etype :    0 -> e is not used
                            1 -> e is initial value for log-periodogram
                            2 -> e is floor periodogram in db
        double   e     : initial value for log-periodogram
                         or floor periodogram in db 
        double   f     : mimimum value of the determinant 
                         of the normal matrix
        int      itype : input data type
        
        return   value :  0 -> completed by end condition
                          -1-> completed by maximum iteration

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

/***************************************************************

  Warping Function and Its Derivative

      double   warp(w, a, t)  &  derivw(w, a, t)

      double  w     : frequency
      double  a     : all-pass constant
      double  t     : emphasized frequency

***************************************************************/

static double warp(const double w, const double a, const double t)
{
   double ww, x, y;

   x = w - t;
   y = w + t;

   ww = w + atan2((a * sin(x)), (1.0 - a * cos(x)))
       + atan2((a * sin(y)), (1.0 - a * cos(y)));

   return (ww);
}


/*============================================================*/

static double derivw(const double w, const double a, const double t)
{
   double dw, x, y, a2, aa;

   x = w - t;
   y = w + t;

   a2 = a + a;
   aa = a * a;

   dw = 1.0 + (a * cos(x) - aa) / (1.0 - a2 * cos(x) + aa)
       + (a * cos(y) - aa) / (1.0 - a2 * cos(y) + aa);

   return (dw);
}

/***************************************************************

  No.1  frqt_a    static : *l, size1

  Frequency Transformation of "al" (second term of dE/dc)

      void frqt_a(al, m, fftsz, a, t)

      double *al   : sequence which will be warped
      int m        : order of warped sequence
      int fftsz    : ifft size
      double a     : all-pass constant
      double t     : emphasized frequency (t * pi)

***************************************************************/

static void frqt_a(double *al, const int m, const int fftsz, const double a,
                   const double t)
{
   int i, j;
   double w, b, *ww, *f, *re, *im, *pf, *pl, *next;
   int size_l, size_f, fftsz2;
   static double *l = NULL;
   static int size1, flag_l = 1;

   b = M_2PI / (double) fftsz;

   size_l = m + 1;

   if (l == NULL) {
      flag_l = 0;
      size1 = size_l;
      l = dgetmem(size1);
   } else if (size_l != size1) {
      free(l);
      flag_l = 0;
      size1 = size_l;
      l = dgetmem(size1);
   }

   /*-------  if "l" is not defined  ----------*/

   if (flag_l == 0) {

      ww = dgetmem(fftsz);

      for (j = 0, w = 0.0; j < fftsz; j++, w += b)
         ww[j] = warp(w, a, t);

      fftsz2 = fftsz + fftsz;   /* size of (re + im) */
      size_f = (m + 1) * fftsz2;        /* size of array "f" */
      f = dgetmem(size_f);

      for (i = 0, re = f, im = f + fftsz; i <= m; i++) {

         for (j = 0; j < fftsz; j++)
            *(re++) = cos(ww[j] * i);
         for (j = 0; j < fftsz; j++)
            *(im++) = -sin(ww[j] * i);

         re -= fftsz;
         im -= fftsz;

         ifft(re, im, fftsz);

         re += fftsz2;
         im += fftsz2;
      }

      free(ww);


      /*-------  copy "f" to "l" ----------*/

      for (i = 0, next = f, pf = f, pl = l; i <= m; i++) {
         *(pl++) = *pf;
         next += fftsz2;
         pf = next;
      }

      free(f);
      flag_l = 1;
   }

   movem(l, al, sizeof(*al), m + 1);

   return;
}

/***************************************************************

  No.2  freqt2    static : *g, size2

  Frequency Transformation

      void freqt2(c1, m1, c2, m2, fftsz, a, t)

      double *c1   : minimum phase sequence
      int    m1    : order of minimum phase sequence
      double *c2   : warped sequence
      int    m2    : order of warped sequence
      int    fftsz : ifft size
      double a     : all-pass constant
      double t     : emphasized frequency (t * pi)

***************************************************************/

static void freqt2(double *c1, const int m1, double *c2, const int m2,
                   const int fftsz, const double a, const double t)
{
   int i, j;
   double w, b, *ww, *dw, *f, *re, *im, *pf, *pg, *next;
   int size_g, size_f, fftsz2;
   static double *g = NULL;
   static int size2, flag_g = 1;

   b = M_2PI / (double) fftsz;

   size_g = (m2 + 1) * (m1 + 1);

   if (g == NULL) {
      flag_g = 0;
      size2 = size_g;
      g = dgetmem(size2);
   } else if (size_g != size2) {
      free(g);
      flag_g = 0;
      size2 = size_g;
      g = dgetmem(size2);
   }

   /*-------  if "g" is not defined  ----------*/

   if (flag_g == 0) {
      ww = dgetmem(fftsz);
      dw = dgetmem(fftsz);

      for (j = 0, w = 0.0; j < fftsz; j++, w += b)
         ww[j] = warp(w, a, t);

      for (j = 0, w = 0.0; j < fftsz; j++, w += b)
         dw[j] = derivw(w, a, t);


      fftsz2 = fftsz + fftsz;   /* size of (re + im) */
      size_f = (m2 + 1) * fftsz2;       /* size of array "f" */
      f = dgetmem(size_f);

      for (i = 0, re = f, im = f + fftsz; i <= m2; i++) {

         for (j = 0; j < fftsz; j++)
            *(re++) = cos(ww[j] * i) * dw[j];
         for (j = 0; j < fftsz; j++)
            *(im++) = -sin(ww[j] * i) * dw[j];

         re -= fftsz;
         im -= fftsz;

         ifft(re, im, fftsz);

         for (j = 1; j <= m1; j++)
            re[j] += re[fftsz - j];

         re += fftsz2;
         im += fftsz2;
      }

      free(ww);
      free(dw);


      /*-------  copy "f" to "g" ----------*/

      for (i = 0, next = f, pf = f, pg = g; i <= m2; i++) {
         for (j = 0; j <= m1; j++)
            *(pg++) = *(pf++);
         next += fftsz2;
         pf = next;
      }
      free(f);
      flag_g = 1;

      for (j = 1; j <= m1; j++)
         g[j] *= 0.5;

      for (i = 1; i <= m2; i++)
         g[i * (m1 + 1)] *= 2.0;
   }

   for (i = 0, pg = g; i <= m2; i++)
      for (j = 0, c2[i] = 0.0; j <= m1; j++)
         c2[i] += *(pg++) * c1[j];

   return;
}



/***************************************************************

  No.3  ifreqt2    static : *h, size3

  Inverse Frequency Transformation

      void ifreqt2(c1, m1, c2, m2, fftsz, a, t)

      double *c1   : minimum phase sequence
      int    m1    : order of minimum phase sequence
      double *c2   : warped sequence
      int    m2    : order of warped sequence
      int    fftsz : ifft size
      double a     : all-pass constant
      double t     : emphasized frequency t * pi(rad)

***************************************************************/

static void ifreqt2(double *c1, int m1, double *c2, int m2, int fftsz, double a,
                    double t)
{
   int i, j;
   double w, b, *ww, *f, *re, *im, *pl, *pr, *plnxt, *prnxt, *pf, *ph, *next;
   int size_h, size_f, fftsz2, m12, m11;
   static double *h = NULL;
   static int size3, flag_h = 1;

   b = M_2PI / (double) fftsz;

   size_h = (m2 + 1) * (m1 + 1);

   if (h == NULL) {
      flag_h = 0;
      size3 = size_h;
      h = dgetmem(size3);
   } else if (size_h != size3) {
      free(h);
      flag_h = 0;
      size3 = size_h;
      h = dgetmem(size3);
   }

   /*-------  if "h" is not defined  ----------*/

   if (flag_h == 0) {
      ww = dgetmem(fftsz);

      for (j = 0, w = 0.0; j < fftsz; j++, w += b)
         ww[j] = warp(w, a, t);

      fftsz2 = fftsz + fftsz;   /* size of (re + im) */

      m12 = m1 + m1 + 1;
      size_f = m12 * fftsz2;    /* size of array "f" */
      f = dgetmem(size_f);

      for (i = -m1, re = f, im = f + fftsz; i <= m1; i++) {

         for (j = 0; j < fftsz; j++)
            *(re++) = cos(ww[j] * i);

         for (j = 0; j < fftsz; j++)
            *(im++) = -sin(ww[j] * i);

         re -= fftsz;
         im -= fftsz;

         ifft(re, im, fftsz);

         re += fftsz2;
         im += fftsz2;
      }

      free(ww);

      /*------- b'(n,m)=b(n,m)+b(n,-m) ----------*/

      pl = f;
      pr = f + (m12 - 1) * fftsz2;

      for (i = 0, plnxt = pl, prnxt = pr; i < m1; i++) {
         plnxt += fftsz2;
         prnxt -= fftsz2;

         for (j = 0; j <= m2; j++)
            *(pr++) += *(pl++);

         pl = plnxt;
         pr = prnxt;
      }

      /*-------  copy "f" to "h" ----------*/

      m11 = m1 + 1;
      pf = f + m1 * fftsz2;

      for (j = 0, next = pf; j <= m1; j++) {

         next += fftsz2;

         for (i = 0; i <= m2; i++)
            h[m11 * i + j] = *(pf++);

         pf = next;
      }
      free(f);
      flag_h = 1;

      for (j = 1; j <= m1; j++)
         h[j] *= 0.5;

      for (i = 1; i <= m2; i++)
         h[i * m11] *= 2.0;
   }

   for (i = 0, ph = h; i <= m2; i++)
      for (j = 0, c2[i] = 0.0; j <= m1; j++)
         c2[i] += *(ph++) * c1[j];

   return;
}



/***************************************************************

  No.4  frqtr2    static : *k, size4

  Frequency Transformation for Calculating Coefficients

      void frqtr2(c1, m1, c2, m2, fftsz, a, t)

      double *c1   : minimum phase sequence
      int    m1    : order of minimum phase sequence
      double *c2   : warped sequence
      int    m2    : order of warped sequence
      int    fftsz : frame length (fft size)
      double a     : all-pass constant
      double t     : emphasized frequency

***************************************************************/

static void frqtr2(double *c1, int m1, double *c2, int m2, int fftsz, double a,
                   double t)
{
   int i, j;
   double w, b, *ww, *f, *tc2, *re, *im, *pf, *pk, *next;
   int size_k, size_f, fftsz2;
   static double *k = NULL;
   static int size4, flag_k = 1;

   b = M_2PI / (double) fftsz;

   size_k = (m2 + 1) * (m1 + 1);

   if (k == NULL) {
      flag_k = 0;
      size4 = size_k;
      k = dgetmem(size4);
   } else if (size_k != size4) {
      free(k);
      flag_k = 0;
      size4 = size_k;
      k = dgetmem(size4);
   }

   /*-------  if "k" is not defined  ----------*/

   if (flag_k == 0) {

      ww = dgetmem(fftsz);

      for (j = 0, w = 0.0; j < fftsz; j++, w += b)
         ww[j] = warp(w, a, t);

      fftsz2 = fftsz + fftsz;   /* size of (re + im) */
      size_f = (m2 + 1) * fftsz2;       /* size of array "f" */
      f = dgetmem(size_f);

      for (i = 0, re = f, im = f + fftsz; i <= m2; i++) {

         for (j = 0; j < fftsz; j++)
            *(re++) = cos(ww[j] * i);
         for (j = 0; j < fftsz; j++)
            *(im++) = -sin(ww[j] * i);

         re -= fftsz;
         im -= fftsz;

         ifft(re, im, fftsz);

         for (j = 1; j <= m1; j++)
            re[j] += re[fftsz - j];

         re += fftsz2;
         im += fftsz2;
      }

      free(ww);


      /*-------  copy "f" to "k" ----------*/

      for (i = 0, next = f, pf = f, pk = k; i <= m2; i++) {
         for (j = 0; j <= m1; j++)
            *(pk++) = *(pf++);
         next += fftsz2;
         pf = next;
      }
      free(f);
      flag_k = 1;
   }

   tc2 = dgetmem(m2 + 1);       /*  tmp of c2  */

   for (i = 0, pk = k; i <= m2; i++)
      for (j = 0, tc2[i] = 0.0; j <= m1; j++)
         tc2[i] += *(pk++) * c1[j];

   movem(tc2, c2, sizeof(*c2), m2 + 1);

   free(tc2);

   return;
}


int smcep(double *xw, const int flng, double *mc, const int m, const int fftsz,
          const double a, const double t, const int itr1, const int itr2,
          const double dd, const int etype, const double e, const double f,
          const int itype)
{
   int i, j;
   int flag = 0, f2, m2;
   double u, s, eps = 0.0, min, max;
   static double *x = NULL, *y, *c, *d, *al, *b;
   static int size_x, size_d;

   if (etype == 1 && e < 0.0) {
      fprintf(stderr, "smcep : value of e must be e>=0!\n");
      exit(1);
   }

   if (etype == 2 && e >= 0.0) {
      fprintf(stderr, "smcep : value of E must be E<0!\n");
      exit(1);
   }

   if (etype == 1) {
      eps = e;
   }


   if (x == NULL) {
      x = dgetmem(3 * flng);
      y = x + flng;
      c = y + flng;
      size_x = flng;

      d = dgetmem(3 * m + 3);
      al = d + (m + 1);
      b = al + (m + 1);
      size_d = m;
   }
   if (flng > size_x) {
      free(x);
      x = dgetmem(3 * flng);
      y = x + flng;
      c = y + flng;
      size_x = flng;
   }
   if (m > size_d) {
      free(d);
      d = dgetmem(3 * m + 3);
      al = d + (m + 1);
      b = al + (m + 1);
      size_d = m;
   }

   f2 = flng / 2.;
   m2 = m + m;

   movem(xw, x, sizeof(*x), flng);

   switch (itype) {
   case 0:                     /* windowed data sequence */
      fftr(x, y, flng);
      for (i = 0; i < flng; i++) {
         x[i] = x[i] * x[i] + y[i] * y[i] + eps;        /*  periodogram  */
      }
      break;
   case 1:                     /* dB */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = exp((x[i] / 20.0) * log(10.0)); /* dB -> amplitude spectrum */
         x[i] = x[i] * x[i] + eps;      /* amplitude -> periodogram */
      }
      break;
   case 2:                     /* log */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = exp(x[i]);      /* log -> amplitude spectrum */
         x[i] = x[i] * x[i] + eps;      /* amplitude -> periodogram */
      }
      break;
   case 3:                     /* amplitude */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = x[i] * x[i] + eps;      /* amplitude -> periodogram */
      }
      break;
   case 4:                     /* periodogram */
      for (i = 0; i <= flng / 2; i++) {
         x[i] = x[i] + eps;
      }
      break;
   default:
      fprintf(stderr, "smcep : Input type %d is not supported!\n", itype);
      exit(1);
   }
   if (itype > 0) {
      for (i = 1; i < flng / 2; i++)
         x[flng - i] = x[i];
   }

   if (etype == 2 && e < 0.0) {
      max = x[0];
      for (i = 1; i < flng; i++) {
         if (max < x[i])
            max = x[i];
      }
      max = sqrt(max);
      min = max * pow(10.0, e / 20.0);  /* floor is 20*log10(min/max) */
      min = min * min;
      for (i = 0; i < flng; i++) {
         if (x[i] < min)
            x[i] = min;
      }
   }

   for (i = 0; i < flng; i++)
      c[i] = log(x[i]);

   /*  1, (-a), (-a)^2, ..., (-a)^M  */

   al[0] = 1.0;
   for (i = 1; i <= m; i++)
      al[i] = 0.0;

   frqt_a(al, m, fftsz, a, t);


   /*  initial value of cepstrum  */
   ifftr(c, y, flng);           /*  c : IFFT[x]  */

   c[0] /= 2.0;
   c[flng / 2] /= 2.0;
   freqt2(c, f2, mc, m, fftsz, a, t);   /*  mc : mel cep.  */

   s = c[0];

   /*  Newton Raphson method  */
   for (j = 1; j <= itr2; j++) {
      fillz(c, sizeof(*c), flng);
      ifreqt2(mc, m, c, f2, fftsz, a, t);       /*  mc : mel cep.  */

      fftr(c, y, flng);         /*  c, y : FFT[mc]  */
      for (i = 0; i < flng; i++)
         c[i] = x[i] / exp(c[i] + c[i]);
      ifftr(c, y, flng);
      frqtr2(c, f2, c, m2, fftsz, a, t);        /*  c : r(k)  */

      u = c[0];
      if (j >= itr1) {
         if (fabs((u - s) / u) < dd) {
            flag = 1;
            break;
         }
         s = u;
      }

      for (i = 0; i <= m; i++)
         b[i] = c[i] - al[i];
      for (i = 0; i <= m2; i++)
         y[i] = c[i];
      for (i = 0; i <= m2; i += 2)
         y[i] -= c[0];
      for (i = 2; i <= m; i += 2)
         c[i] += c[0];
      c[0] += c[0];

      if (theq(c, y, d, b, m + 1, f)) {
         fprintf(stderr, "smcep : Error in theq() at %dth iteration!\n", j);
         exit(1);
      }

      for (i = 0; i <= m; i++)
         mc[i] += d[i];
   }

   if (flag)
      return (0);
   else
      return (-1);
}
