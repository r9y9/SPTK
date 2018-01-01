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

    GLSA Digital Filter

        double   glsadf(x, c, m, n, d)

        double   x   : input
        double   *c  : filter coefficients (K, g*c'(1), ..., g*c'(m))
        int      m   : order of cepstrum
        int      n   : -1/gamma
        double   *d  : delay

        return value : filtered data

    Inverse GLSA Digital Filter

        double  iglsadf(x, c, m, n, d)

        double  x     : input
        double  *c    : filter coefficients (K, g*c'(1), ..., g*c'(m))
        int     m     : order of cepstrum
        int     n     : -1/gamma
        double  *d    : delay

        return  value : filtered data

*****************************************************************/

#include <stdio.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

/* no option */

static double gpoledf(double x, double *c, int m, const double g, double *d)
{
   double y = 0.0;

   for (m--; m > 0; m--) {
      y -= c[m + 1] * d[m];
      d[m] = d[m - 1];
   }
   y -= c[1] * d[0];
   y *= g;
   d[0] = (x += y);

   return (x);
}

double glsadf(double x, double *c, const int m, const int n, double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = poledf(x, c, m, &d[m * i]);

   return (x);
}

double glsadf1(double x, double *c, const int m, const int n, double *d)
{
   int i;
   double gamma;

   gamma = -1 / (double) n;

   for (i = 0; i < n; i++)
      x = gpoledf(x, c, m, gamma, &d[m * i]);

   return (x);
}

/* inverse option */

static double gzerodf(double x, double *c, int m, const double g, double *d)
{
   double y = 0.0;

   for (m--; m > 0; m--) {
      y += c[m + 1] * d[m];
      d[m] = d[m - 1];
   }
   y += c[1] * d[0];
   d[0] = x;

   x += y * g;

   return (x);
}

double iglsadf(double x, double *c, const int m, const int n, double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = zerodf1(x, c, m, &d[m * i]);

   return (x);
}

double iglsadf1(double x, double *c, const int m, const int n, double *d)
{
   int i;
   double gamma;

   gamma = -1 / (double) n;

   for (i = 0; i < n; i++)
      x = gzerodf(x, c, m, gamma, &d[m * i]);

   return (x);
}

/* transpose option */

static double glsadfft(double x, double *c, const int m, double *d)
{
   int i;

   x -= d[0];
   d[m] = c[m] * x;
   for (i = m - 1; i >= 1; i--)
      d[i] += c[i] * x;

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (x);
}

double glsadft(double x, double *c, const int m, const int n, double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = glsadfft(x, c, m, &d[(m + 1) * i]);
   return (x);
}

static double glsadff1t(double x, double *c, const int m, const double g,
                        double *d)
{
   int i;

   x -= d[0] * g;

   d[m] = c[m] * x;
   for (i = m - 1; i >= 1; i--)
      d[i] += c[i] * x;

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (x);
}

double glsadf1t(double x, double *c, const int m, const int n, double *d)
{
   int i;
   double gamma;

   gamma = -1 / (double) n;

   for (i = 0; i < n; i++)
      x = glsadff1t(x, c, m, gamma, &d[(m + 1) * i]);

   return (x);
}

/* inverse and transpose */

static double iglsadfft(double x, double *c, const int m, double *d)
{
   int i;
   double y;

   y = x + d[0];

   d[m] = c[m] * x;
   for (i = m - 1; i >= 1; i--)
      d[i] += c[i] * x;

   for (i = 0; i < m; i++)
      d[i] = d[i + 1];

   return (y);
}

double iglsadft(double x, double *c, const int m, const int n, double *d)
{
   int i;

   for (i = 0; i < n; i++)
      x = iglsadfft(x, c, m, &d[i * (m + 1)]);

   return (x);
}

static double iglsadff1t(double x, double *c, const int m, const double g,
                         double *d)
{
   int i;
   double y;

   y = x + g * d[0];

   d[m] = c[m] * x;
   for (i = m - 1; i >= 1; i--)
      d[i] += c[i] * x;

   for (i = 0; i < m; i++)
      d[i] = c[i + 1];

   return (y);
}

double iglsadf1t(double x, double *c, const int m, const int n, double *d)
{
   int i;
   double g;

   g = -1.0 / (double) n;

   for (i = 0; i < n; i++)
      x = iglsadff1t(x, c, m, g, &d[i * (m + 1)]);

   return (x);
}
