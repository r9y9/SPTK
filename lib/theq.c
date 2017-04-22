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

/******************************************************************

    $Id: theq.c,v 1.23 2016/12/22 10:53:15 fjst15124 Exp $

    Subroutine for Solving a Toeplitz plus Hankel
    Coefficient Matrix System of Equations  ( T + H ) a = b


    int	theq(t, h, a, b, n, eps)

    double  *t : Toeplitz elements -> T(i,j) = t(|i-j|) t[0]..t[n-1]
    double  *h : Hankel elements -> H(i,j) = h(i+j)     h[0]...h[2*n-2]
    double  *a : solution vector of equation            a[0]...a[n-1]
    double  *b : known vector  	                       b[0]...b[n-1]
    int      n : system order
    double eps : singular check (eps(if -1.0, 1.0e-6 is assumed))

    return value :
          0 : normally completed
         -1 : abnormally completed

****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static void mv_mul(double *t, double *x, double *y)
{
   t[0] = x[0] * y[0] + x[1] * y[1];
   t[1] = x[2] * y[0] + x[3] * y[1];

   return;
}

static void mm_mul(double *t, double *x, double *y)
{
   t[0] = x[0] * y[0] + x[1] * y[2];
   t[1] = x[0] * y[1] + x[1] * y[3];
   t[2] = x[2] * y[0] + x[3] * y[2];
   t[3] = x[2] * y[1] + x[3] * y[3];

   return;
}

static int inverse(double *x, double *y, const double eps)
{
   double det;

   det = y[0] * y[3] - y[1] * y[2];

#ifdef WIN32
   if ((fabs(det) < eps) || _isnan(det)) {
#else
   if ((fabs(det) < eps) || isnan(det)) {
#endif
      fprintf(stderr,
              "theq() : determinant of the normal matrix is too small!\n");
      return (-1);
   }

   x[0] = y[3] / det;
   x[1] = -y[1] / det;
   x[2] = -y[2] / det;
   x[3] = y[0] / det;

   return (0);
}

static void crstrns(double *x, double *y)
{
   x[0] = y[3];
   x[1] = y[2];
   x[2] = y[1];
   x[3] = y[0];

   return;
}

static double **mtrx2(const int a, const int b)
{
   int i;
   double **x;

   if (!(x = (double **) calloc((size_t) a, sizeof(*x)))) {
      fprintf(stderr, "mtrx2() in theq() : Cannot allocate memory!\n");
      exit(3);
   }
   for (i = 0; i < a; i++)
      if (!(x[i] = (double *) calloc((size_t) b, sizeof(**x)))) {
         fprintf(stderr, "mtrx2() in theq() : Cannot allocate memory!\n");
         exit(3);
      }

   return (x);
}

static int cal_p0(double **p, double **r, double *b, const int n,
                  const double eps)
{
   double t[4], s[2];

   if (inverse(t, r[0], eps) == -1)
      return (-1);
   s[0] = b[0];
   s[1] = b[n - 1];
   mv_mul(p[0], t, s);

   return (0);
}

static void cal_ex(double *ex, double **r, double **x, const int i)
{
   int j;
   double t[4], s[4];

   s[0] = s[1] = s[2] = s[3] = 0.;

   for (j = 0; j < i; j++) {
      mm_mul(t, r[i - j], x[j]);
      s[0] += t[0];
      s[1] += t[1];
      s[2] += t[2];
      s[3] += t[3];
   }

   ex[0] = s[0];
   ex[1] = s[1];
   ex[2] = s[2];
   ex[3] = s[3];

   return;
}

static void cal_ep(double *ep, double **r, double **p, const int i)
{
   int j;
   double t[2], s[2];

   s[0] = s[1] = 0.;

   for (j = 0; j < i; j++) {
      mv_mul(t, r[i - j], p[j]);
      s[0] += t[0];
      s[1] += t[1];
   }
   ep[0] = s[0];
   ep[1] = s[1];

   return;
}

static int cal_bx(double *bx, double *vx, double *ex, const double eps)
{
   double t[4], s[4];

   crstrns(t, vx);
   if (inverse(s, t, eps) == -1)
      return (-1);
   mm_mul(bx, s, ex);

   return (0);
}

static void cal_x(double **x, double **xx, double *bx, const int i)
{
   int j;
   double t[4], s[4];

   for (j = 1; j < i; j++) {
      crstrns(t, xx[i - j]);
      mm_mul(s, t, bx);
      x[j][0] -= s[0];
      x[j][1] -= s[1];
      x[j][2] -= s[2];
      x[j][3] -= s[3];
   }

   for (j = 1; j < i; j++) {
      xx[j][0] = x[j][0];
      xx[j][1] = x[j][1];
      xx[j][2] = x[j][2];
      xx[j][3] = x[j][3];
   }

   x[i][0] = xx[i][0] = -bx[0];
   x[i][1] = xx[i][1] = -bx[1];
   x[i][2] = xx[i][2] = -bx[2];
   x[i][3] = xx[i][3] = -bx[3];

   return;
}

static void cal_vx(double *vx, double *ex, double *bx)
{
   double t[4], s[4];

   crstrns(t, ex);
   mm_mul(s, t, bx);
   vx[0] -= s[0];
   vx[1] -= s[1];
   vx[2] -= s[2];
   vx[3] -= s[3];

   return;
}

static int cal_g(double *g, double *vx, double *b, double *ep,
                 const int i, const int n, const double eps)
{
   double t[2], s[4], u[4];

   t[0] = b[i] - ep[0];
   t[1] = b[n - 1 - i] - ep[1];
   crstrns(s, vx);

   if (inverse(u, s, eps) == -1)
      return (-1);
   mv_mul(g, u, t);

   return (0);
}

static void cal_p(double **p, double **x, double *g, const int i)
{
   double t[4], s[2];
   int j;

   for (j = 0; j < i; j++) {
      crstrns(t, x[i - j]);
      mv_mul(s, t, g);
      p[j][0] += s[0];
      p[j][1] += s[1];
   }

   p[i][0] = g[0];
   p[i][1] = g[1];

   return;
}

int theq(double *t, double *h, double *a, double *b, const int n, double eps)
{
   static double **r = NULL, **x, **xx, **p;
   static int size;
   double ex[4], ep[2], vx[4], bx[4], g[2];
   int i;

   if (r == NULL) {
      r = mtrx2(n, 4);
      x = mtrx2(n, 4);
      xx = mtrx2(n, 4);
      p = mtrx2(n, 2);
      size = n;
   }
   if (n > size) {
      for (i = 0; i < size; i++) {
         free((char *) r[i]);
         free((char *) x[i]);
         free((char *) xx[i]);
         free((char *) p[i]);
      }
      free((char *) r);
      free((char *) x);
      free((char *) xx);
      free((char *) p);

      r = mtrx2(n, 4);
      x = mtrx2(n, 4);
      xx = mtrx2(n, 4);
      p = mtrx2(n, 2);
      size = n;
   }

   if (eps < 0.0)
      eps = 1.0e-6;

   /* make r */
   for (i = 0; i < n; i++) {
      r[i][0] = r[i][3] = t[i];
      r[i][1] = h[n - 1 + i];
      r[i][2] = h[n - 1 - i];
   }

   /* step 1 */
   x[0][0] = x[0][3] = 1.0;
   if (cal_p0(p, r, b, n, eps) == -1)
      return (-1);

   vx[0] = r[0][0];
   vx[1] = r[0][1];
   vx[2] = r[0][2];
   vx[3] = r[0][3];

   /* step 2 */
   for (i = 1; i < n; i++) {
      cal_ex(ex, r, x, i);
      cal_ep(ep, r, p, i);
      if (cal_bx(bx, vx, ex, eps) == -1)
         return (-1);
      cal_x(x, xx, bx, i);
      cal_vx(vx, ex, bx);
      if (cal_g(g, vx, b, ep, i, n, eps) == -1)
         return (-1);
      cal_p(p, x, g, i);
   }

   /* step 3 */
   for (i = 0; i < n; i++)
      a[i] = p[i][0];

   return (0);
}
