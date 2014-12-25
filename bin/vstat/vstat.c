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
/*                1996-2014  Nagoya Institute of Technology          */
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

/************************************************************************
*                                                                       *
*    Vector Statistics Calculation                                      *
*                                                                       *
*                                      1998.12 T.Masuko                 *
*                                      2010.8  A.Tamamori               *
*                                      2012.11 T.Okada                  *
*       usage:                                                          *
*               vstat [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -l l     :  length of vector                   [1]      *
*               -n n     :  order of vector                    [l-1]    *
*               -t t     :  number of vector                   [all]    *
*               -o o     :  output format                      [0]      *
*                             0 mean & covariance                       *
*                             1 mean                                    *
*                             2 covariance                              *
*                             3 mean and upper / lower bound of         *
*                               confidence interval via t-dist.         *
*                             4 median                                  *
*               -c c     :  conf. level of conf. interval      [0.95]   *
*               -d       :  diagonal covariance                [FALSE]  *
*               -i       :  output inverse cov. instead of cov.[FALSE]  *
*               -r       :  output correlation instead of cov. [FALSE]  *
*       infile:                                                         *
*              data sequence                                            *
*                      x_1(1), ..., x_1(L), x_2(1) ...                  *
*       stdout:                                                         *
*              mean vector                                              *
*                      m(1), ..., m(L),                                 *
*              covariance matrix                                        *
*                      U(11), ..., m(1L),                               *
*                      ...............,                                 *
*                      U(L1), ..., m(LL), ...                           *
*              upper/lower bound of confidence interval                 *
*                      upper(1), ..., upper(L),                         *
*                      lower(1), ..., lower(L)                          *
*       notice:                                                         *
*              if '-d' is specified,                                    *
*              off-diagonal elements are suppressed.                    *
*              '-d' and '-r' are exclusive                              *
*              ('-r' has priority over '-d').                           *
*       require:                                                        *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: vstat.c,v 1.37 2014/12/11 08:30:51 uratec Exp $";


/*  Standard C Libralies  */
#include <stdio.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define LENG    1
#define OUTMEAN TR
#define OUTCOV  TR
#define OUTCONF FA
#define OUTMED  FA
#define CONFLEV 0.9500E2
#define DIAGC   FA
#define INV     FA
#define CORR    FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - vector statistics calculation\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l   : length of vector                    [%d]\n",
           LENG);
   fprintf(stderr,
           "       -n n   : order of vector                     [l-1]\n");
   fprintf(stderr,
           "       -t t   : number of vector                    [N/A]\n");
   fprintf(stderr, "       -o o   : output format                       [0]\n");
   fprintf(stderr, "                  0 mean & covariance\n");
   fprintf(stderr, "                  1 mean\n");
   fprintf(stderr, "                  2 covariance\n");
   fprintf(stderr, "                  3 mean and upper / lower bound of\n");
   fprintf(stderr, "                    confidence interval via t-dist.\n");
   fprintf(stderr, "                  4 median\n");
   fprintf(stderr,
           "       -c c   : conf. level of conf. interval (%%)   [%g]\n",
           CONFLEV);
   fprintf(stderr, "       -d     : diagonal covariance                 [%s]\n",
           BOOL[DIAGC]);
   fprintf(stderr, "       -i     : output inverse cov. instead of cov. [%s]\n",
           BOOL[INV]);
   fprintf(stderr, "       -r     : output correlation instead of cov.  [%s]\n",
           BOOL[CORR]);
   fprintf(stderr, "       -h     : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       vectors (%s)                [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       mean(s) and covariance(s) of input vectors (%s)\n",
           FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr,
           "       if '-d' is specified, off-diagonal elements are suppressed.\n");
   fprintf(stderr,
           "       '-d' and '-r' are exclusive ('-r' has priority over '-d').\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

double norm_percent(const double alpha)
{
   int i;
   double a, x, u;
   static double b[11] = {
      1.570796288, 0.03706987906, -0.8364353589e-3,
      -0.2250947176e-3, 0.6841218299e-5, 0.5824238515e-5,
      -0.104527497e-5, 0.8360937017e-7, -0.3231081277e-8,
      0.3657763036e-10, 0.6936233982e-12
   };

   if (alpha == 0.5) {
      return 0.0;
   }

   a = alpha;
   if (a > 0.5) {
      a = 1.0 - a;
   }
   x = -log(4.0 * a * (1.0 - a));
   u = b[0];
   for (i = 1; i < 11; i++) {
      u += (b[i] * pow(x, (double) i));
   }
   if (alpha > 0.5) {
      return -sqrt(u * x);
   } else {
      return sqrt(u * x);
   }
}

double t_lower(const double t, const int df)
{
   int i;
   double c2, p, s;

   c2 = df / (df + t * t);
   s = sqrt(1 - c2);
   if (t < 0) {
      s = -s;
   }
   p = 0;
   for (i = df % 2 + 2; i <= df; i += 2) {
      p += s;
      s *= (i - 1) * c2 / i;
   }
   if (df & 1) {
      return (0.5 + (p * sqrt(c2) + atan(t / sqrt(df))) / PI);
   } else {
      return ((1 + p) / 2);
   }
}

double t_percent(const double p, const int df)
{
   double f, y1, y2, y3, y4, y5, q, r, u, u2, x, delta;
   int n;

   q = (1.0 - p) / 2;
   f = (double) df;

   u = norm_percent(q);
   u2 = u * u;

   y1 = (u2 + 1) / 4;
   y2 = ((5 * u2 + 16) * u2 + 3) / 96;
   y3 = (((3 * u2 + 19) * u2 + 17) * u2 - 15) / 384;
   y4 = ((((79 * u2 + 776) * u2 + 1482) * u2 - 1920) * u2 - 945) / 92160;
   y5 = (((((27 * u2 + 339) * u2 + 930) * u2 - 1782) * u2 - 765) * u2 +
         17955) / 368640;

   x = u * (1 + (y1 + (y2 + (y3 + (y4 + y5 / f) / f) / f) / f) / f);

   if (df <= pow(log10(1 - q), 2) + 3) {
      r = t_lower(x, df);
      n = df + 1;
      delta = (r - (1 - q))
          / exp((n * log((double) n / (df + x * x))
                 + log((double) df / n / 2 / PI) - 1
                 + (1.0 / n - 1.0 / df) / 6.0) / 2.0);
      x -= delta;
   }

   return (x);
}

void quicksort(double *x, int left, int right)
{
   int i, j;
   int pivot;
   int tmp;

   i = left;
   j = right;

   pivot = x[(left + right) / 2];

   while (1) {

      while (x[i] < pivot)
         i++;

      while (pivot < x[j])
         j--;
      if (i >= j)
         break;

      tmp = x[i];
      x[i] = x[j];
      x[j] = tmp;

      i++;
      j--;
   }

   if (left < i - 1)
      quicksort(x, left, i - 1);
   if (j + 1 < right)
      quicksort(x, j + 1, right);
}

int main(int argc, char *argv[])
{
   FILE *fp = stdin;
   double *x, *mean, *med = NULL, **mtmp = NULL, **cov = NULL, **invcov =
       NULL, *var = NULL, conf = CONFLEV, *upper = NULL, *lower = NULL, t, err;
   int leng = LENG, nv = -1, i, j, k = 0, lp = 0, m, outtype = 0, count = 0;
   Boolean outmean = OUTMEAN, outcov = OUTCOV, outconf = OUTCONF,
       outmed = OUTMED, diagc = DIAGC, inv = INV, corr = CORR;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            leng = atoi(*++argv);
            --argc;
            break;
         case 'n':
            leng = atoi(*++argv) + 1;
            --argc;
            break;
         case 't':
            nv = atoi(*++argv);
            --argc;
            break;
         case 'o':
            outtype = atoi(*++argv);
            --argc;
            break;
         case 'c':
            conf = atof(*++argv);
            --argc;
            break;
         case 'd':
            diagc = 1 - diagc;
            break;
         case 'i':
            inv = 1 - inv;
            break;
         case 'r':
            corr = 1 - corr;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   if (conf < 0 || conf > 100) {
      fprintf(stderr,
              "%s : Confidence level must be greater than 0 and less than 1.0!\n",
              cmnd);
   }
   switch (outtype) {
   case 1:
      outcov = FA;
      break;
   case 2:
      outmean = FA;
      break;
   case 3:
      outcov = FA;
      outconf = TR;
      break;
   case 4:
      outcov = FA;
      outmean = FA;
      outmed = TR;
      break;
   }
   if (diagc && corr)
      diagc = FA;
   if (diagc && inv)
      diagc = FA;
   if (corr && inv)
      corr = FA;

   mean = dgetmem(leng + leng);

   x = mean + leng;

   if (outmed) {

      if (nv == -1) {
         typedef struct _float_list {
            float *f;
            struct _float_list *next;
         } float_list;

         float_list *top = NULL, *prev = NULL, *cur = NULL;
         top = prev = (float_list *) getmem(1, sizeof(float_list));
         prev->next = NULL;
         while (freadf(x, sizeof(*x), leng, fp) == leng) {
            cur = (float_list *) getmem(1, sizeof(float_list));
            cur->f = fgetmem(leng);
            for (i = 0; i < leng; i++) {
               cur->f[i] = (float) x[i];
            }
            count++;
            prev->next = cur;
            cur->next = NULL;
            prev = cur;
         }
         k = count;

         mtmp = (double **) getmem(leng, sizeof(*mtmp));
         mtmp[0] = dgetmem(leng * k);
         for (i = 1; i < leng; i++)
            mtmp[i] = mtmp[i - 1] + k;

         med = dgetmem(leng);

         for (j = 0, cur = top->next; j < k; j++, cur = cur->next) {
            for (i = 0; i < leng; i++) {
               mtmp[i][j] = (double) cur->f[i];
            }
         }
      } else {

         k = nv;

         mtmp = (double **) getmem(leng, sizeof(*mtmp));
         mtmp[0] = dgetmem(leng * k);
         for (i = 1; i < leng; i++)
            mtmp[i] = mtmp[i - 1] + k;

         med = dgetmem(leng);

         for (j = 0; j < k; j++) {
            for (i = 0; i < leng; i++) {
               freadf(&mtmp[i][j], sizeof(**mtmp), 1, fp);
            }
         }
      }

      if (k % 2 == 0) {
         fprintf(stderr, "%s : warning: the number of vectors is even!\n",
                 cmnd);
      }

      for (i = 0; i < leng; i++) {
         quicksort(mtmp[i], 0, k - 1);
         if (k % 2 == 1) {
            med[i] = mtmp[i][k / 2];
         } else {
            med[i] = ((mtmp[i][k / 2] + mtmp[i][k / 2 - 1]) / 2);
         }
      }

      fwritef(med, sizeof(*med), leng, stdout);

      return (0);

   }

   if (outcov) {
      if (!diagc) {
         cov = (double **) getmem(leng, sizeof(*cov));
         cov[0] = dgetmem(leng * leng);
         for (i = 1; i < leng; i++)
            cov[i] = cov[i - 1] + leng;

         if (inv) {
            invcov = (double **) getmem(leng, sizeof(*invcov));
            invcov[0] = dgetmem(leng * leng);
            for (i = 1; i < leng; i++)
               invcov[i] = invcov[i - 1] + leng;
         }
      } else
         var = dgetmem(leng);
   }
   if (outconf) {
      var = dgetmem(leng);
      upper = dgetmem(leng);
      lower = dgetmem(leng);
   }



   while (!feof(fp)) {
      for (i = 0; i < leng; i++) {
         mean[i] = 0.0;
         if (outcov) {
            if (!diagc)
               for (j = 0; j < leng; j++)
                  cov[i][j] = 0.0;
            else
               var[i] = 0.0;
         }
         if (outconf) {
            var[i] = 0.0;
         }
      }

      for (lp = nv; lp;) {
         if (freadf(x, sizeof(*x), leng, fp) != leng)
            break;
         for (i = 0; i < leng; i++) {
            mean[i] += x[i];
            if (outcov) {
               if (!diagc)
                  for (j = i; j < leng; j++)
                     cov[i][j] += x[i] * x[j];
               else
                  var[i] += x[i] * x[i];
            }
            if (outconf) {
               var[i] += x[i] * x[i];
            }
         }
         --lp;
      }

      if (lp == 0 || nv == -1) {
         if (nv > 0)
            k = nv;
         else
            k = -lp - 1;
         for (i = 0; i < leng; i++)
            mean[i] /= k;
         if (outcov) {
            if (!diagc)
               for (i = 0; i < leng; i++)
                  for (j = i; j < leng; j++)
                     cov[j][i] = cov[i][j] = cov[i][j] / k - mean[i] * mean[j];
            else
               for (i = 0; i < leng; i++)
                  var[i] = var[i] / k - mean[i] * mean[i];
         }
         if (outconf) {
            for (i = 0; i < leng; i++) {
               var[i] = (var[i] - k * mean[i] * mean[i]) / (k - 1);
            }
            t = t_percent(conf / 100, k - 1);
            for (i = 0; i < leng; i++) {
               err = t * sqrt(var[i] / k);
               upper[i] = mean[i] + err;
               lower[i] = mean[i] - err;
            }
         }

         if (corr) {
            for (i = 0; i < leng; i++)
               for (j = i + 1; j < leng; j++)
                  cov[j][i] = cov[i][j] =
                      cov[i][j] / sqrt(cov[i][i] * cov[j][j]);
            for (i = 0; i < leng; i++)
               cov[i][i] = 1.0;
         }

         if (outmean)
            fwritef(mean, sizeof(*mean), leng, stdout);

         if (outcov) {
            if (!diagc) {
               if (inv) {
                  for (i = 0; i < leng; i++) {
                     for (j = i + 1; j < leng; j++) {
                        cov[j][i] /= cov[i][i];
                        for (m = i + 1; m < leng; m++)
                           cov[j][m] -= cov[i][m] * cov[j][i];
                     }
                  }

                  for (m = 0; m < leng; m++) {
                     for (i = 0; i < leng; i++) {
                        if (i == m)
                           invcov[i][m] = 1.0;
                        else
                           invcov[i][m] = 0.0;
                     }
                     for (i = 0; i < leng; i++) {
                        for (j = i + 1; j < leng; j++)
                           invcov[j][m] -= invcov[i][m] * cov[j][i];
                     }
                     for (i = leng - 1; i >= 0; i--) {
                        for (j = i + 1; j < leng; j++)
                           invcov[i][m] -= cov[i][j] * invcov[j][m];
                        invcov[i][m] /= cov[i][i];
                     }
                  }
                  fwritef(invcov[0], sizeof(*invcov[0]), leng * leng, stdout);
               } else
                  fwritef(cov[0], sizeof(*cov[0]), leng * leng, stdout);
            } else
               fwritef(var, sizeof(*var), leng, stdout);
         }
         if (outconf) {
            fwritef(upper, sizeof(*upper), leng, stdout);
            fwritef(lower, sizeof(*lower), leng, stdout);
         }
      }

   }

   return (0);
}
