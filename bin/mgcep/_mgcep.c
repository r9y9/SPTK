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

   $Id: _mgcep.c,v 1.32 2016/12/22 10:53:08 fjst15124 Exp $

   Mel-Generalized Cepstral Analysis

       int mgcep(xw, flng, b, m, a, g, n, itr1, itr2, dd, etype, e, f, itype);

       double   *xw   : input sequence
       int      flng  : frame length
       double   *b    : coefficient b'(m)
       int      m     : order of mel cepstrum
       double   a     : alpha
       double   g     : gamma
       int      n     : order of recursions
       int      itr1  : minimum number of iteration
       int      itr2  : maximum number of iteration
       double   dd    : end condition
       int      etype :       0 -> e is not used
                              1 -> e is initial value for log-periodogram
                              2 -> e is floor periodogram in db
       double   e     : value for log-periodogram
                        or floor periodogram in db
       double   f     : mimimum value of the determinant 
                        of the normal matrix
       int      itype : input data type
                         
       return value   : 0 -> completed by end condition
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

/*  gain(epsilon) calculation  */
static double gain(double *er, double *c, int m, double g)
{
   int i;
   double t;

   if (g != 0.0) {
      for (t = 0.0, i = 1; i <= m; i++)
         t += er[i] * c[i];
      return (er[0] + g * t);
   } else
      return (er[0]);
}

/*  b'(m) to c(m)  */
static void b2c(double *b, int m1, double *c, int m2, double a)
{
   int i, j;
   static double *d = NULL, *g;
   static int size;
   double k;

   if (d == NULL) {
      size = m2;
      d = dgetmem(size + size + 2);
      g = d + size + 1;
   }
   if (m2 > size) {
      free(d);
      size = m2;
      d = dgetmem(size + size + 2);
      g = d + size + 1;
   }

   k = 1 - a * a;

   fillz(g, sizeof(*g), m2 + 1);

   for (i = -m1; i <= 0; i++) {
      d[0] = g[0];
      g[0] = b[-i];

      if (1 <= m2)
         g[1] = k * d[0] + a * (d[1] = g[1]);

      for (j = 2; j <= m2; j++)
         g[j] = d[j - 1] + a * ((d[j] = g[j]) - g[j - 1]);
   }
   movem(g, c, sizeof(*g), m2 + 1);

   return;
}

/*  recursion for p(m)  */
static void ptrans(double *p, int m, double a)
{
   double d, o;

   d = p[m];
   for (m--; m > 0; m--) {
      o = p[m] + a * d;
      d = p[m];
      p[m] = o;
   }
   o = a * d;
   p[m] = (1. - a * a) * p[m] + o + o;

   return;
}

/*  recursion for q(m)  */
static void qtrans(double *q, int m, double a)
{
   int i;
   double d, o;

   m += m;
   i = 1;
   d = q[i];
   for (i++; i <= m; i++) {
      o = q[i] + a * d;
      d = q[i];
      q[i] = o;
   }

   return;
}

int mgcep(double *xw, int flng, double *b, const int m, const double a,
          const double g, const int n, const int itr1, const int itr2,
          const double dd, const int etype, const double e, const double f,
          const int itype)
{
   int i, j, flag = 0;
   static double *x = NULL, *y, *d;
   static int size_x, size_c;
   double ep, epo, eps = 0.0, min, max;

   if (etype == 1 && e < 0.0) {
      fprintf(stderr, "mgcep : value of e must be e>=0!\n");
      exit(1);
   }

   if (etype == 2 && e >= 0.0) {
      fprintf(stderr, "mgcep : value of E must be E<0!\n");
      exit(1);
   }

   if (etype == 1) {
      eps = e;
   }

   if (x == NULL) {
      x = dgetmem(flng + flng);
      y = x + flng;
      size_x = flng;
      d = dgetmem(m + 1);
      size_c = m;
   }
   if (flng > size_x) {
      free(x);
      x = dgetmem(flng + flng);
      y = x + flng;
      size_x = flng;
   }
   if (m > size_c) {
      free(d);
      d = dgetmem(m + 1);
      size_c = m;
   }

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
      fprintf(stderr, "mgcep : Input type %d is not supported!\n", itype);
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

   /* initial value */
   fillz(b, sizeof(*b), m + 1);
   ep = newton(x, flng, b, m, a, -1.0, n, 0, f);

   if (g != -1.0) {
      if (a != 0.0) {
         ignorm(b, b, m, -1.0); /*  K, b'r(m)    -> br(m)         */
         b2mc(b, b, m, a);      /*  br(m)        -> c~r(m)        */
         gnorm(b, d, m, -1.0);  /*  c~r(m)       -> K~, c~'r(m)   */
      } else
         movem(b, d, sizeof(*b), m + 1);

      gc2gc(d, m, -1.0, b, m, g);       /*  K~, c~'r(m)  -> K~, c~'r'(m)  */

      if (a != 0.0) {
         ignorm(b, b, m, g);    /*  K~, c~'r'(m) -> c~r(m)        */
         mc2b(b, b, m, a);      /*  c~r(m)       -> br(m)         */
         gnorm(b, b, m, g);     /*  br(m)        -> K, b'r'(m)    */
      }
   }

   /*  Newton-Raphson method  */
   if (g != -1.0) {
      for (j = 1; j <= itr2; j++) {
         epo = ep;
         ep = newton(x, flng, b, m, a, g, n, j, f);

         if (j >= itr1)
            if (fabs((epo - ep) / ep) < dd) {
               flag = 1;
               break;
            }
      }
   }

   if (flag)
      return (0);
   else
      return (-1);
}

double newton(double *x, const int flng, double *c, const int m, const double a,
              const double g, const int n, const int j, const double f)
{
   int i, m2;
   double t = 0, s, tr, ti, trr, tii;
   static double *cr = NULL, *ci, *pr, *qr, *qi, *rr, *ri, *b;
   static int size_cr, size_b;

   if (cr == NULL) {
      cr = dgetmem(7 * flng);
      ci = cr + flng;
      pr = ci + flng;
      qr = pr + flng;
      qi = qr + flng;
      rr = qi + flng;
      ri = rr + flng;
      size_cr = flng;

      b = dgetmem(m + 1);
      size_b = m;
   }
   if (flng > size_cr) {
      free(cr);
      cr = dgetmem(7 * flng);
      ci = cr + flng;
      pr = ci + flng;
      qr = pr + flng;
      qi = qr + flng;
      rr = qi + flng;
      ri = rr + flng;
      size_cr = flng;
   }
   if (m > size_b) {
      free(b);
      b = dgetmem(m + 1);
      size_b = m;
   }

   m2 = m + m;

   fillz(cr, sizeof(*cr), flng);
   movem(&c[1], &cr[1], sizeof(*c), m);

   if (a != 0.0)
      b2c(cr, m, cr, n, -a);

   fftr(cr, ci, flng);          /* cr +j ci : FFT[c]  */

   if (g == -1.0)
      movem(x, pr, sizeof(*x), flng);
   else if (g == 0.0)
      for (i = 0; i < flng; i++)
         pr[i] = x[i] / exp(cr[i] + cr[i]);
   else
      for (i = 0; i < flng; i++) {
         tr = 1 + g * cr[i];
         ti = g * ci[i];
         s = (trr = tr * tr) + (tii = ti * ti);
         t = x[i] * pow(s, -1.0 / g);
         pr[i] = (t /= s);
         rr[i] = tr * t;
         ri[i] = ti * t;
         t /= s;
         qr[i] = (trr - tii) * t;
         s = tr * ti * t;
         qi[i] = s + s;
      }

   ifftr(pr, ci, flng);

   if (a != 0.0)
      b2c(pr, n, pr, m2, a);

   if (g == 0.0 || g == -1.0) {
      movem(pr, qr, sizeof(*pr), m2 + 1);
      movem(pr, rr, sizeof(*pr), m + 1);
   } else {
      ifft(qr, qi, flng);
      ifft(rr, ri, flng);

      if (a != 0.0) {
         b2c(qr, n, qr, n, a);
         b2c(rr, n, rr, m, a);
      }
   }

   if (a != 0.0) {
      ptrans(pr, m, a);
      qtrans(qr, m, a);
   }

   /*  c[0] : gain, t : epsilon  */
   if (g != -1.0)
      c[0] = sqrt(t = gain(rr, c, m, g));

   if (g == -1.0)
      fillz(qr, sizeof(*qr), m2 + 1);
   else if (g != 0.0)
      for (i = 2; i <= m2; i++)
         qr[i] *= 1.0 + g;

   if (theq(pr, &qr[2], &b[1], &rr[1], m, f)) {
      fprintf(stderr, "mgcep : Error in theq() at %dth iteration!\n", j);
      exit(1);
   }

   for (i = 1; i <= m; i++)
      c[i] += b[i];

   /*  c[0] : gain, t : epsilon  */
   if (g == -1.0)
      c[0] = sqrt(t = gain(rr, c, m, g));

   return (log(t));
}
