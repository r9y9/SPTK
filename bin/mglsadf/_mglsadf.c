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
/*                1996-2017  Nagoya Institute of Technology          */
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

    $Id$

    MGLSA Digital Filter

        double mglsadf(x, b, m, a, n, d)

        double  x     : input
        double  *b    : filter coefficients (K, g*b'(1), ..., g*b'(m))
        int     m     : order of cepstrum
        double  a     : alpha
        int     n     : -1/gamma
        double  *d    : delay

        return  value : filtered data

    Inverse MGLSA Digital Filter

        double   imglsadf(x, b, m, a, n, d)

        double   x     : input
        double   *b    : filter coefficients (K, g*b'(1), ..., g*b'(m))
        int      m     : order of cepstrum
        double   a     : alpha
        int      n     : -1/gamma
        double   *d    : delay

        return   value : filtered data

*****************************************************************/

#include <stdio.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

static double mglsadff(double x, double *b, const int m, const double a,
                       double *d)
{
   int i;
   double y, aa;

   aa = 1 - a * a;

   y = d[0] * b[1];
   for (i = 1; i < m; i++) {
      d[i] += a * (d[i + 1] - d[i - 1]);
      y += d[i] * b[i + 1];
   }
   x -= y;

   for (i = m; i > 0; i--)
      d[i] = d[i - 1];
   d[0] = a * d[0] + aa * x;

   return (x);
}

double mglsadf(double x, double *b, const int m, const double a, const int n,
               double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = mglsadff(x, b, m, a, &d[i * (m + 1)]);

   return (x);
}

static double mglsadff1(double x, double *b, const int m, const double a,
                        const double g, double *d)
{
   int i;
   double y, aa;

   aa = 1 - a * a;

   y = d[0] * b[1];
   for (i = 1; i < m; i++) {
      d[i] += a * (d[i + 1] - d[i - 1]);
      y += d[i] * b[i + 1];
   }
   x -= g * y;

   for (i = m; i > 0; i--)
      d[i] = d[i - 1];

   d[0] = a * d[0] + aa * x;

   return (x);
}

double mglsadf1(double x, double *b, const int m, const double a, const int n,
                double *d)
{
   int i;
   double g;

   g = -1.0 / (double) n;

   for (i = 0; i < n; i++)
      x = mglsadff1(x, b, m, a, g, &d[i * (m + 1)]);

   return (x);
}

static double mglsadfft(double x, double *b, const int m, const double a,
                        double *d)
{
   int i;

   x -= d[0] * (1.0 - a * a);

   d[m] = b[m] * x + a * d[m - 1];
   for (i = m - 1; i >= 1; i--)
      d[i] += b[i] * x + a * (d[i - 1] - d[i + 1]);

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (x);
}

double mglsadft(double x, double *b, const int m, const double a, const int n,
                double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = mglsadfft(x, b, m, a, &d[i * (m + 1)]);

   return (x);
}

static double mglsadff1t(double x, double *b, const int m, const double a,
                         const double g, double *d)
{
   int i;

   x -= d[0] * (1.0 - a * a) * g;

   d[m] = b[m] * x + a * d[m - 1];
   for (i = m - 1; i >= 1; i--)
      d[i] += b[i] * x + a * (d[i - 1] - d[i + 1]);

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (x);
}

double mglsadf1t(double x, double *b, const int m, const double a, const int n,
                 double *d)
{
   int i;
   double g;

   g = -1.0 / (double) n;

   for (i = 0; i < n; i++)
      x = mglsadff1t(x, b, m, a, g, &d[i * (m + 1)]);

   return (x);
}

static double imglsadff(double x, double *b, const int m, const double a,
                        double *d)
{
   int i;
   double y, aa;

   aa = 1 - a * a;

   y = d[0] * b[1];
   for (i = 1; i < m; i++) {
      d[i] += a * (d[i + 1] - d[i - 1]);
      y += d[i] * b[i + 1];
   }
   y += x;

   for (i = m; i > 0; i--)
      d[i] = d[i - 1];

   d[0] = a * d[0] + aa * x;

   return (y);
}

double imglsadf(double x, double *b, const int m, const double a, const int n,
                double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = imglsadff(x, b, m, a, &d[i * (m + 1)]);

   return (x);
}

static double imglsadff1(double x, double *b, const int m, const double a,
                         const double g, double *d)
{
   int i;
   double y, aa;

   aa = 1 - a * a;

   y = d[0] * b[1];
   for (i = 1; i < m; i++) {
      d[i] += a * (d[i + 1] - d[i - 1]);
      y += d[i] * b[i + 1];
   }
   y = g * y + x;

   for (i = m; i > 0; i--)
      d[i] = d[i - 1];

   d[0] = a * d[0] + aa * x;

   return (y);
}

double imglsadf1(double x, double *b, const int m, const double a, const int n,
                 double *d)
{
   int i;
   double g;

   g = -1.0 / (double) n;

   for (i = 0; i < n; i++)
      x = imglsadff1(x, b, m, a, g, &d[i * (m + 1)]);

   return (x);
}

static double imglsadfft(double x, double *b, const int m, const double a,
                         double *d)
{
   int i;
   double y;

   y = x + (1.0 - a * a) * d[0];

   d[m] = b[m] * x + a * d[m - 1];
   for (i = m - 1; i >= 1; i--)
      d[i] += b[i] * x + a * (d[i - 1] - d[i + 1]);

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (y);
}

double imglsadft(double x, double *b, const int m, const double a, const int n,
                 double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = imglsadfft(x, b, m, a, &d[i * (m + 1)]);

   return (x);
}

static double imglsadff1t(double x, double *b, const int m, const double a,
                          const double g, double *d)
{
   int i;
   double y;

   y = x + g * (1.0 - a * a) * d[0];

   d[m] = b[m] * x + a * d[m - 1];
   for (i = m - 1; i >= 1; i--)
      d[i] += b[i] * x + a * (d[i - 1] - d[i + 1]);

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (y);
}

double imglsadf1t(double x, double *b, const int m, const double a, const int n,
                  double *d)
{
   int i;
   double g;

   g = -1.0 / (double) n;

   for (i = 0; i < n; i++)
      x = imglsadff1t(x, b, m, a, g, &d[i * (m + 1)]);

   return (x);
}
