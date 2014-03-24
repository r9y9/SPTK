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
/*                1996-2012  Nagoya Institute of Technology          */
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

/*************************************************************************
 *                                                                       *
 *    Gaussian Mixture Model                                             *
 *                                                                       *
 *                                       2000.7   C. Miyajima            *
 *                                       2010.4   A. Saito               *
 *                                       2010.9   A. Tamamori            *
 *                                                                       *
 *       usage:                                                          *
 *               gmm [options] [infile] > stdout                         *
 *       options:                                                        *
 *               -l l  :  length of vector                    [26]       *
 *               -m m  :  number of Gaussian components       [16]       *
 *               -s s  :  seed of random var. for LBG algo.   [1]        *
 *               -a a  :  minimum number of EM iterations     [0]        *
 *               -b b  :  maximum number of EM iterations     [20]       *
 *               -e e  :  convergence factor                  [0.00001]  *
 *               -v v  :  floor value of variances            [0.001]    *
 *               -w w  :  floor value of weights (1/m)*w      [0.001]    *
 *               -f    :  full covariance                     [FALSE]    *
 *               -F fn :  gmm initial parameter file          [FALSE]    *
 *       infile:                                                         *
 *               training data sequence                       [stdin]    *
 *       stdout:                                                         *
 *               GMM parameters                                          *
 *                                                                       *
 ************************************************************************/

static char *rcs_id = "$Id: gmm.c,v 1.17 2012/12/18 12:41:08 mataki Exp $";

/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

#include "gmm.h"

/*  Default Values  */
#define DEF_L       26
#define DEF_M       16
#define DEF_T       -1
#define DEF_S       1
#define DEF_IMIN    0
#define DEF_IMAX    20
#define DEF_E       0.00001
#define DEF_V       0.001
#define DEF_W       0.001
#define DELTA       0.0001
#define END         0.0001
#define EPSILON     1.0e-6
#define MAXVALUE    1.0e10
#define FULL        0

/* Default values for lbg */
#define ITER 1000
#define CENTUP 1
#define MINTRAIN 1

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - EM estimation of GMM\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : dimension of vector                [%d]\n",
           DEF_L);
   fprintf(stderr, "       -m m  : number of Gaussian components      [%d]\n",
           DEF_M);
   fprintf(stderr, "       -s s  : seed of random var. for LBG algo.  [%d]\n",
           DEF_S);
   fprintf(stderr, "       -a a  : minimum number of EM iterations    [%d]\n",
           DEF_IMIN);
   fprintf(stderr, "       -b b  : maximum number of EM iterations    [%d]\n",
           DEF_IMAX);
   fprintf(stderr, "       -e e  : convergence factor                 [%g]\n",
           DEF_E);
   fprintf(stderr, "       -v v  : flooring value for variance        [%g]\n",
           DEF_V);
   fprintf(stderr, "       -w w  : flooring value for weights (1/m)*w [%g]\n",
           DEF_W);
   fprintf(stderr, "       -f    : full covariance                    [%s]\n",
           BOOL[FULL]);
   fprintf(stderr, "       -F fn : GMM initial parameter file         [N/A]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       training data sequence (float)             [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       GMM parameters (float)\n");
   fprintf(stderr, "         output order:\n");
   fprintf(stderr,
           "            weight-1, weight-2, ..., weight-m, mean_vec-1, variance_vec-1,\n");
   fprintf(stderr,
           "            mean_vec-2, variance_vec-2, ..., mean_vec-m, variance_vec-m\n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr,
           "       -e option specifies a threshold for the change of average\n");
   fprintf(stderr,
           "         log-likelihood for training data at each iteration.\n");
   fprintf(stderr,
           "       -F option specifies a GMM initial parameter file in which\n");
   fprintf(stderr,
           "         weight, mean, and variance parameters must be aligned\n");
   fprintf(stderr, "         in the same order as output.\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

int main(int argc, char **argv)
{
   FILE *fp = stdin, *fgmm = NULL;
   GMM gmm, tgmm, floor;
   double E = DEF_E, V = DEF_V, W = DEF_W,
       *dat, *pd, *cb, *icb, *logwgd, logb, *sum, *sum_m, **sum_v, diff, sum_w,
       ave_logp0 = 0.0, ave_logp1, change = MAXVALUE, tmp1, tmp2;
   int l, L = DEF_L, m, M = DEF_M, N, t, T = DEF_T, S =
       DEF_S, full = FULL, n1 = 0, i, j, Imin = DEF_IMIN, Imax =
       DEF_IMAX, *tindex, *cntcb;
   void cal_inv(double **cov, double **inv, const int L);
   float_list *top, *cur, *prev, *tmpf, *tmpff;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   /* --  Check options -- */
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'h':
            usage(0);
            break;
         case 'l':
            L = atoi(*++argv);
            --argc;
            break;
         case 'm':
            M = atoi(*++argv);
            --argc;
            break;
         case 's':
            S = atoi(*++argv);
            --argc;
            break;
         case 'a':
            Imin = atoi(*++argv);
            --argc;
            break;
         case 'b':
            Imax = atoi(*++argv);
            --argc;
            break;
         case 'e':
            E = atof(*++argv);
            --argc;
            break;
         case 'v':
            V = atof(*++argv);
            --argc;
            break;
         case 'w':
            W = atof(*++argv);
            --argc;
            break;
         case 'f':
            full = 1 - full;
            break;
         case 'F':
            fgmm = getfp(*++argv, "rb");
            --argc;
            break;
         default:
            fprintf(stderr, "%s: Illegal option \"%s\".\n", cmnd, *argv);
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   /* -- Count number of input vectors and read -- */
   dat = dgetmem(L);
   top = prev = (float_list *) malloc(sizeof(float_list));
   top->f = fgetmem(L);
   T = 0;
   prev->next = NULL;
   while (freadf(dat, sizeof(*dat), L, fp) == L) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = fgetmem(L);
      for (i = 0; i < L; i++) {
         cur->f[i] = (float) dat[i];
      }
      T++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }
   free(dat);
   dat = dgetmem(L * T);
   for (i = 0, tmpf = top->next; tmpf != NULL; i++, tmpf = tmpff) {
      for (j = 0; j < L; j++) {
         dat[i * L + j] = tmpf->f[j];
      }
      tmpff = tmpf->next;
      free(tmpf->f);
      free(tmpf);
   }
   free(top);

   /* GMM */
   gmm.weight = dgetmem(M);
   gmm.gauss = (Gauss *) getmem(M, sizeof(Gauss));

   for (m = 0; m < M; m++) {
      gmm.gauss[m].mean = dgetmem(L);
      gmm.gauss[m].var = dgetmem(L);

      if (full == 1) {
         gmm.gauss[m].cov = (double **) malloc(sizeof(double *) * L);
         gmm.gauss[m].inv = (double **) malloc(sizeof(double *) * L);
         for (l = 0; l < L; l++) {
            gmm.gauss[m].cov[l] = dgetmem(L);
            gmm.gauss[m].inv[l] = dgetmem(L);
         }
      }
   }

   floor.gauss = NULL;
   if (full == 1) {
      floor.gauss = (Gauss *) getmem(1, sizeof(Gauss));
      floor.gauss[0].cov = (double **) malloc(sizeof(double *) * L);
      for (l = 0; l < L; l++)
         floor.gauss[0].cov[l] = dgetmem(L);
      sum_m = dgetmem(L);
      sum_v = (double **) malloc(sizeof(double *) * L);
   }
   /* temporary */
   tgmm.weight = dgetmem(M);
   tgmm.gauss = (Gauss *) getmem(M, sizeof(Gauss));

   for (m = 0; m < M; m++) {
      tgmm.gauss[m].mean = dgetmem(L);
      tgmm.gauss[m].var = dgetmem(L);

      if (full == 1) {
         tgmm.gauss[m].cov = (double **) malloc(sizeof(double *) * L);
         tgmm.gauss[m].inv = (double **) malloc(sizeof(double *) * L);
         for (l = 0; l < L; l++) {
            tgmm.gauss[m].cov[l] = dgetmem(L);
            tgmm.gauss[m].inv[l] = dgetmem(L);
         }
      }
   }

   logwgd = dgetmem(M);
   sum = dgetmem(M);

   /* Initialization of GMM parameters */
   if (fgmm != NULL) {
      fprintf(stderr, "T = %d  L = %d  M = %d\n", T, L, M);

      /* load GMM parameter */
      freadf(gmm.weight, sizeof(double), M, fgmm);
      if (full != 1) {          /* diagonal */
         for (m = 0; m < M; m++) {
            freadf(gmm.gauss[m].mean, sizeof(double), L, fgmm);
            freadf(gmm.gauss[m].var, sizeof(double), L, fgmm);
         }
      } else {
         for (m = 0; m < M; m++) {
            freadf(gmm.gauss[m].mean, sizeof(double), L, fgmm);
            for (l = 0; l < L; l++)
               freadf(gmm.gauss[m].cov[l], sizeof(double), L, fgmm);
         }
      }
      fclose(fgmm);
   } else {
      /* for VQ */
      N = 1;
      while (N < M)
         N *= 2;
      cb = dgetmem(N * L);
      icb = dgetmem(L);
      tindex = (int *) getmem(T, sizeof(int));
      cntcb = (int *) getmem(M, sizeof(int));

      /* LBG */
      vaverage(dat, L, T, icb);
      lbg(dat, L, T, icb, 1, cb, N, ITER, MINTRAIN, S, CENTUP, DELTA, END);

      for (t = 0, pd = dat; t < T; t++, pd += L) {
         tindex[t] = vq(pd, cb, L, M);
         cntcb[tindex[t]]++;
      }

      for (m = 0; m < M; m++)
         if (cntcb[m] == 0) {
            fprintf(stderr, "Error: No data for mixture No.%d\n", m);
            usage(1);
         }

      fprintf(stderr, "T = %d  L = %d  M = %d\n", T, L, M);

      /* flooring value for weights */
      W = 1.0 / (double) M *(double) W;

      /* weights */
      for (m = 0, sum_w = 0.0; m < M; m++) {
         gmm.weight[m] = (double) cntcb[m] / (double) T;
         if (gmm.weight[m] < W)
            gmm.weight[m] = W;
         sum_w += gmm.weight[m];
      }
      if (sum_w != 1.0)
         for (m = 0; m < M; m++)
            gmm.weight[m] /= sum_w;


      /* mean */
      for (m = 0, pd = cb; m < M; m++, pd += L)
         movem(pd, gmm.gauss[m].mean, sizeof(double), L);


      /* variance */
      if (full != 1) {
         for (t = 0, pd = dat; t < T; t++, pd += L)
            for (l = 0; l < L; l++) {
               diff = gmm.gauss[tindex[t]].mean[l] - pd[l];
               gmm.gauss[tindex[t]].var[l] += sq(diff);
            }

         for (m = 0; m < M; m++)
            for (l = 0; l < L; l++) {
               gmm.gauss[m].var[l] /= (double) cntcb[m];
               if (gmm.gauss[m].var[l] < V)
                  gmm.gauss[m].var[l] = V;
            }

         for (m = 0; m < M; m++)
            gmm.gauss[m].gconst = cal_gconst(gmm.gauss[m].var, L);

      }
      /* full covariance */
      else {
         for (t = 0, pd = dat; t < T; t++, pd += L) {
            for (l = 0; l < L; l++) {
               for (i = 0; i <= l; i++) {
                  if (l == i) {
                     diff =
                         (gmm.gauss[tindex[t]].mean[l] -
                          pd[l]) * (gmm.gauss[tindex[t]].mean[i] - pd[i]);
                     floor.gauss[0].cov[l][i] += diff;
                  }
               }
            }
         }

         for (l = 0; l < L; l++) {
            for (i = 0; i <= l; i++) {
               if (l == i) {
                  floor.gauss[0].cov[l][i] /= T;
                  floor.gauss[0].cov[l][i] *= V;
               }
            }
         }

         for (t = 0, pd = dat; t < T; t++, pd += L) {
            for (l = 0; l < L; l++) {
               for (i = 0; i <= l; i++) {
                  diff =
                      (gmm.gauss[tindex[t]].mean[l] -
                       pd[l]) * (gmm.gauss[tindex[t]].mean[i] - pd[i]);
                  gmm.gauss[tindex[t]].cov[l][i] += diff;
               }
            }
         }

         for (m = 0; m < M; m++)
            for (l = 0; l < L; l++)
               for (i = 0; i <= l; i++) {
                  gmm.gauss[m].cov[l][i] /= (double) cntcb[m];
               }
      }
   }                            /* end of initialization */

   /* EM training of GMM parameters */
   for (i = 0; (i <= Imax) && ((i <= Imin) || (fabs(change) > E)); i++) {
      if (full != 1)
         fillz_gmm(&tgmm, M, L);
      else
         fillz_gmmf(&tgmm, M, L);
      fillz(sum, sizeof(double), M);

      if (full != 1) {
         for (m = 0; m < M; m++)
            gmm.gauss[m].gconst = cal_gconst(gmm.gauss[m].var, L);
      } else {
         for (m = 0, n1 = 0; m < M; m++) {
            gmm.gauss[m].gconst = cal_gconstf(gmm.gauss[m].cov, L);

            if (gmm.gauss[m].gconst == 0) {
               n1++;
               for (l = 0; l < L; l++)
                  gmm.gauss[m].cov[l][l] += floor.gauss[0].cov[l][l];
               gmm.gauss[m].gconst = cal_gconstf(gmm.gauss[m].cov, L);
            }
            if (gmm.gauss[m].gconst == 0) {
               fprintf(stderr, "ERROR : Can't caluculate covdet");
               exit(EXIT_FAILURE);
            }

            /* calculate inv */
            cal_inv(gmm.gauss[m].cov, gmm.gauss[m].inv, L);
         }
      }
      if (full == 1)
         fprintf(stderr, "%d cov can't caluculate covdet\n", n1);

      for (t = 0, ave_logp1 = 0.0, pd = dat; t < T; t++, pd += L) {
         for (m = 0, logb = LZERO; m < M; m++) {
            if (full != 1) {
               logwgd[m] = log_wgd(&gmm, m, pd, L);
               logb = log_add(logb, logwgd[m]);
            }
            /* full */
            else {
               logwgd[m] = log_wgdf(&gmm, m, pd, L);
               logb = log_add(logb, logwgd[m]);
            }
         }
         ave_logp1 += logb;

         for (m = 0; m < M; m++) {
            tmp1 = exp(logwgd[m] - logb);
            sum[m] += tmp1;

            for (l = 0; l < L; l++) {
               tmp2 = tmp1 * pd[l];
               tgmm.gauss[m].mean[l] += tmp2;
               if (full != 1)
                  tgmm.gauss[m].var[l] += tmp2 * pd[l];
               else {
                  for (j = 0; j <= l; j++) {
                     tgmm.gauss[m].cov[l][j] +=
                         tmp1 * (pd[l] - gmm.gauss[m].mean[l]) * (pd[j] -
                                                                  gmm.
                                                                  gauss[m].mean
                                                                  [j]);
                  }
               }
            }
         }
      }

      /* Output average log likelihood at each iteration */
      ave_logp1 /= (double) T;
      if (i == 1 && m == 1)
         ave_logp0 = ave_logp1;

      fprintf(stderr, "iter %3d : ", i);
      fprintf(stderr, "ave_logprob = %g", ave_logp1);
      if (i) {
         change = ave_logp1 - ave_logp0;
         fprintf(stderr, "  change = %g", change);
      }
      fprintf(stderr, "\n");
      ave_logp0 = ave_logp1;

      /* Update perameters */
      /* weights */
      for (m = 0; m < M; m++)
         gmm.weight[m] = sum[m] / (double) T;

      /* mean, variance */
      for (m = 0; m < M; m++) {
         for (l = 0; l < L; l++)
            gmm.gauss[m].mean[l] = tgmm.gauss[m].mean[l] / sum[m];

         if (full != 1) {
            for (l = 0; l < L; l++) {
               gmm.gauss[m].var[l] =
                   tgmm.gauss[m].var[l] / sum[m] - sq(gmm.gauss[m].mean[l]);
               if (gmm.gauss[m].var[l] < V)
                  gmm.gauss[m].var[l] = V;
            }
         }
         /* full */
         else {
            for (l = 0; l < L; l++) {
               for (j = 0; j <= l; j++) {
                  gmm.gauss[m].cov[l][j] = tgmm.gauss[m].cov[l][j] / sum[m];
               }
            }
         }
      }
   }

   /*  Output GMM parameters */
   fwritef(gmm.weight, sizeof(double), M, stdout);
   if (full != 1) {
      for (m = 0; m < M; m++) {
         fwritef(gmm.gauss[m].mean, sizeof(double), L, stdout);
         fwritef(gmm.gauss[m].var, sizeof(double), L, stdout);
      }
   } else {
      for (m = 0; m < M; m++) {
         fwritef(gmm.gauss[m].mean, sizeof(double), L, stdout);
         for (i = 0; i < L; i++)
            for (j = 0; j < i; j++)
               gmm.gauss[m].cov[j][i] = gmm.gauss[m].cov[i][j];
         for (l = 0; l < L; l++)
            fwritef(gmm.gauss[m].cov[l], sizeof(double), L, stdout);
      }
   }
   return (0);
}
