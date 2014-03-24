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
/*                1996-2013  Nagoya Institute of Technology          */
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

/*********************************************************************************
*                                                                                *
*    PCA : Principal Component Analysis                                          *
*                                                                                *
*                                           2009.8 A.Tamamori                    *
*                                                                                *
*       usage:                                                                   *
*                pca [ options ] [ infile ] > stdout                             *
*                                                                                *
*       options:                                                                 *
*                -l L  : dimentionality of input/output vectors      [3]         *
*                -n N  : number of output principal componets        [2]         *
*                -i I  : number of iteration of Jacobi method        [10000]     *
*                -e e  : threshold of convergence of Jacobi method   [0.000001]  *
*                -v    : output eigenvectors and mean vector         [FALSE]     *
*                -V fn : output eigenvalues and contribution ratio               *
*                        (output filename = fn)                      [NULL]      *
*                                                                                *
*       infile:                                                                  *
*                training vectors                                                *
*                    X(0), X(1), X(2), ...                                       *
*                    where                                                       *  
*                        X(0)=[x(0), x(1), ..., x(L-1)],                         *
*                        X(1)=[x(L), x(L+1), ..., x(2*L-1)],                     *
*                        X(2)=[x(2*L), x(L+1), ..., x(3*L-1)],                   *
*                          .                                                     *
*                          .                                                     *
*                          .                                                     *
*                                                                                *
*       stdout:                                                                  *
*                eigenvectors and mean vector (if -v option is specified)        *
*                   mean_vec, evec(1), evec(2), ..., evec(N)                     *
*                                                                                *
*       notice:                                                                  *
*                Calculation of PCA is based on jacobi method.                   *
*                The Order of Output eigen values and contribution ration is:    *
*                   eig_val-1, contri_ratio-1, ..., eig_val-N, contri_ratio-N    *
*                                                                                *
*********************************************************************************/

static char *rcs_id = "$Id: pca.c,v 1.20 2013/12/16 09:02:02 mataki Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/* Defalut Values */
#define LENG           24
#define PRICOMP_ORDER  2
#define EPS            0.000001
#define ITEMAX         10000

typedef enum { FALSE = -1, TRUE = 1 } BOOL;

/* Command Name */
char *cmnd;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - Pricipal Component Analysis\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -l L  : dimentionality of input vectors               [%d]\n",
           LENG);
   fprintf(stderr,
           "       -n N  : dimentionality of output vectors              [%d]\n",
           PRICOMP_ORDER);
   fprintf(stderr,
           "       -i I  : iteration of Jacobi method                    [%d]\n",
           ITEMAX);
   fprintf(stderr,
           "       -e e  : threshold of convergence of Jacobi method     [%f]\n",
           EPS);
   fprintf(stderr,
           "       -v    : output eigen vectors and mean vector          [FALSE]\n");
   fprintf(stderr,
           "       -V fn : output eigen values and contribution ratio \n");
   fprintf(stderr,
           "               (output filename = fn)                        [FALSE]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       training data set       [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       mean vector and eigen vectors       [stdin]\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

double **malloc_matrix(int n)
{
   double **m;
   double *mtmp;
   int i;

   mtmp = dgetmem(n * n);
   if ((m = (double **) malloc(sizeof(double *) * n)) == NULL) {
      fprintf(stderr, "Can't malloc in %s\n", cmnd);
      exit(EXIT_FAILURE);
   }
   for (i = 0; i < n; i++) {
      m[i] = &(mtmp[i * n]);
   }
   return m;
}

int jacobi(double **m, int n, double eps, double *e_val, double **e_vec,
           int itemax)
{
   int i, j, k;
   int count;
   int ret;
   double **a;
   double max_e;
   int r = 0, c = 0;
   double a1, a2, a3;
   double co, si;
   double w1, w2;
   double t1, ta;
   double tmp;

   for (i = 0; i < n; i++) {
      for (j = i + 1; j < n; j++) {
         if (m[i][j] != m[j][i]) {
            return -1;
         }
      }
   }

   if ((a = malloc_matrix(n)) == NULL) {
      fprintf(stderr, "Error : Can't malloc at jacobi in %s\n", cmnd);
      exit(EXIT_FAILURE);
   }
   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
         a[i][j] = m[i][j];
      }
   }

   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
         e_vec[i][j] = (i == j) ? 1.0 : 0.0;
      }
   }

   count = 0;

   while (1) {

      max_e = 0.0;
      for (i = 0; i < n; i++) {
         for (j = i + 1; j < n; j++) {
            if (max_e < fabs(a[i][j])) {
               max_e = fabs(a[i][j]);
               r = i;
               c = j;
            }
         }
      }
      if (max_e <= eps) {
         ret = count;
         break;
      }
      if (count >= itemax) {
         ret = -2;
         break;
      }

      a1 = a[r][r];
      a2 = a[c][c];
      a3 = a[r][c];

      t1 = fabs(a1 - a2);
      ta = 2.0 * a3 / (t1 + sqrt(t1 * t1 + 4.0 * a3 * a3));
      co = sqrt(1.0 / (ta * ta + 1.0));
      si = ta * co;
      if (a1 < a2)
         si = -si;

      for (i = 0; i < n; i++) {
         w1 = e_vec[i][r];
         w2 = e_vec[i][c];
         e_vec[i][r] = w1 * co + w2 * si;
         e_vec[i][c] = -w1 * si + w2 * co;
         if (i == r || i == c)
            continue;
         w1 = a[i][r];
         w2 = a[i][c];
         a[i][r] = w1 * co + w2 * si;
         a[i][c] = -w1 * si + w2 * co;
         a[r][i] = a[i][r];
         a[c][i] = a[i][c];
      }
      a[r][r] = a1 * co * co + a2 * si * si + 2.0 * a3 * co * si;
      a[c][c] = a1 + a2 - a[r][r];
      a[r][c] = 0.0;
      a[c][r] = 0.0;

      count++;
   }

   for (i = 0; i < n; i++) {
      e_val[i] = a[i][i];
   }

   for (i = 0; i < n; i++) {
      for (j = i + 1; j < n; j++) {
         tmp = e_vec[i][j];
         e_vec[i][j] = e_vec[j][i];
         e_vec[j][i] = tmp;
      }
   }

   for (i = 0; i < n; i++) {
      for (j = n - 2; j >= i; j--) {
         if (e_val[j] < e_val[j + 1]) {
            tmp = e_val[j];
            e_val[j] = e_val[j + 1];
            e_val[j + 1] = tmp;
            for (k = 0; k < n; k++) {
               tmp = e_vec[j][k];
               e_vec[j][k] = e_vec[j + 1][k];
               e_vec[j + 1][k] = tmp;
            }
         }
      }
   }

   free(a[0]);
   free(a);

   return (ret);
}

int main(int argc, char *argv[])
{
   FILE *fp = stdin, *fp_eigen = NULL;
   int i, j, k, n = PRICOMP_ORDER, leng = LENG, total = -1;
   BOOL out_evecFlg = FALSE, out_evalFlg = FALSE;
   double sum;
   double *buf = NULL;
   double *mean = NULL, **var = NULL;
   double eps = EPS;
   int itemax = ITEMAX;
   double **e_vec = NULL, *e_val = NULL;        /* eigenvector and eigenvalue */
   double *cont_rate = NULL;    /* contribution rate */
   double jacobi_conv;
   float_list *top, *cur, *prev, *tmpf, *tmpff;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc)
      if ((**++argv) == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            leng = atoi(*++argv);
            --argc;
            break;
         case 'n':
            n = atoi(*++argv);
            --argc;
            break;
         case 'e':
            eps = atof(*++argv);
            --argc;
            break;
         case 'i':
            itemax = atoi(*++argv);
            --argc;
            break;
         case 'v':
            out_evecFlg = TRUE;
            break;
         case 'V':
            out_evalFlg = TRUE;
            fp_eigen = getfp(*++argv, "wb");
            --argc;
            break;
         case 'h':
            usage(EXIT_SUCCESS);
         default:
            fprintf(stderr, "%s : Invalid option '%s'!\n", cmnd, *argv);
            usage(EXIT_FAILURE);
         }
      } else
         fp = getfp(*argv, "rb");

   if (n > leng) {
      fprintf(stderr, "\n %s (Error) output number of pricipal component"
              " must be less than length of vector.\n", cmnd);
      usage(EXIT_FAILURE);
   }

   /* -- Count number of input vectors and read -- */
   buf = dgetmem(leng);
   top = prev = (float_list *) malloc(sizeof(float_list));
   top->f = fgetmem(leng);
   total = 0;
   prev->next = NULL;
   while (freadf(buf, sizeof(*buf), leng, fp) == leng) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = fgetmem(leng);
      for (i = 0; i < leng; i++) {
         cur->f[i] = (float) buf[i];
      }
      total++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }
   free(buf);
   buf = dgetmem(total * leng);
   for (i = 0, tmpf = top->next; tmpf != NULL; i++, tmpf = tmpff) {
      for (j = 0; j < leng; j++) {
         buf[i * leng + j] = tmpf->f[j];
      }
      tmpff = tmpf->next;
      free(tmpf->f);
      free(tmpf);
   }
   free(top);

/* PCA */
   /* allocate memory for mean vectors and covariance matrix */
   mean = dgetmem(leng);
   var = malloc_matrix(leng);

   /* calculate mean vector */
   for (i = 0; i < leng; i++) {
      for (j = 0, sum = 0.0; j < total; j++)
         sum += buf[i + j * leng];
      mean[i] = sum / total;
   }
   /* calculate cov. mat. */
   for (i = 0; i < leng; i++) {
      for (j = 0; j < leng; j++) {
         sum = 0.0;
         for (k = 0; k < total; k++)
            sum +=
                (buf[i + k * leng] - mean[i]) * (buf[j + k * leng] - mean[j]);
         var[i][j] = sum / total;
      }
   }

   /* allocate memory for eigenvector and eigenvalue */
   e_vec = malloc_matrix(leng);
   e_val = dgetmem(leng);

   /* calculate eig.vec. and eig.val. with jacobi method */
   if ((jacobi_conv = jacobi(var, leng, eps, e_val, e_vec, itemax)) == -1) {
      fprintf(stderr, "Error : matrix is not symmetric.\n");
      exit(EXIT_FAILURE);
   } else if (jacobi_conv == -2) {
      fprintf(stderr, "Error : loop in jacobi method reached %d times.\n",
              itemax);
      exit(EXIT_FAILURE);
   }

   /* allocate memory for contribution rate of each eigenvalue */
   cont_rate = dgetmem(leng);

   /* calculate contribution rate of each eigenvalue */
   for (j = 0; j < leng; j++) {
      sum = 0.0;
      for (i = 0; i < leng; i++)
         sum += e_val[i];
      cont_rate[j] = e_val[j] / sum;
   }
/* end of PCA */

   /* output mean vector and eigen vectors */
   if (out_evecFlg == TRUE) {
      fwritef(mean, sizeof(*mean), leng, stdout);
      for (i = 0; i < n; i++)
         fwritef(e_vec[i], sizeof(*(e_vec[i])), leng, stdout);
   }

   /* output eigen values and contribution ratio */
   if (out_evalFlg == TRUE) {
      for (i = 0; i < n; i++) {
         fwritef(e_val + i, sizeof(*e_val), 1, fp_eigen);
         fwritef(cont_rate + i, sizeof(*cont_rate), 1, fp_eigen);
      }
      fclose(fp_eigen);
   }

   return EXIT_SUCCESS;
}
