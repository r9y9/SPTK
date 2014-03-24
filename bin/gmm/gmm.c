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

/***************************************************************************
 *                                                                         *
 *    Gaussian Mixture Model                                               *
 *                                                                         *
 *                                       2000.7   C. Miyajima              *
 *                                       2010.4   A. Saito                 *
 *                                       2010.9   A. Tamamori              *
 *                                                                         *
 *       usage:                                                            *
 *               gmm [options] [infile] > stdout                           *
 *       options:                                                          *
 *               -l l  :  length of vector                      [26]       *
 *               -m m  :  number of Gaussian components         [16]       *
 *               -s s  :  seed of random var. for LBG algo.     [1]        *
 *               -a a  :  minimum number of EM iterations       [0]        *
 *               -b b  :  maximum number of EM iterations       [20]       *
 *               -e e  :  convergence factor                    [0.00001]  *
 *               -v v  :  floor value of variances              [0.001]    *
 *               -w w  :  floor value of weights (1/m)*w        [0.001]    *
 *               -f    :  full covariance                       [FALSE]    *
 *               -F fn :  gmm initial parameter file            [FALSE]    *
 *               -M M1 ... Mb : block size in covariance matrix [FALSE]    *
 *               -c1   : inter-block correlation                [FALSE]    *
 *               -c2   : full covariance in each block          [FALSE]    *
 *       infile:                                                           *
 *               training data sequence                         [stdin]    *
 *       stdout:                                                           *
 *               GMM parameters                                            *
 *                                                                         *
 **************************************************************************/

static char *rcs_id = "$Id: gmm.c,v 1.22 2013/12/16 09:01:57 mataki Exp $";

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
#include <ctype.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

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
   fprintf(stderr,
           "       -l l  : dimension of vector                         [%d]\n",
           DEF_L);
   fprintf(stderr,
           "       -m m  : number of Gaussian components               [%d]\n",
           DEF_M);
   fprintf(stderr,
           "       -s s  : seed of random var. for LBG algo.           [%d]\n",
           DEF_S);
   fprintf(stderr,
           "       -a a  : minimum number of EM iterations             [%d]\n",
           DEF_IMIN);
   fprintf(stderr,
           "       -b b  : maximum number of EM iterations             [%d]\n",
           DEF_IMAX);
   fprintf(stderr,
           "       -e e  : convergence factor                          [%g]\n",
           DEF_E);
   fprintf(stderr,
           "       -v v  : flooring value for variance                 [%g]\n",
           DEF_V);
   fprintf(stderr,
           "       -w w  : flooring value for weights (1/m)*w          [%g]\n",
           DEF_W);
   fprintf(stderr,
           "       -f    : full covariance                             [%s]\n",
           BOOL[FULL]);
   fprintf(stderr,
           "       -F fn : GMM initial parameter file                  [N/A]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "     (level 2)\n");
   fprintf(stderr,
           "       -M M1 M2 ... Mb : block size in covariance matrix,  [FALSE]\n");
   fprintf(stderr, "                         where (M1 + M2 + ... + Mb) = l\n");
   fprintf(stderr,
           "       -c1   : inter-block correlation                     [FALSE]\n");
   fprintf(stderr,
           "       -c2   : full covariance in each block               [FALSE]\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       training data sequence (float)                      [stdin]\n");
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
   fprintf(stderr,
           "       -M option specifies the size of each blocks in covariance matrix.\n");
   fprintf(stderr, "       -c1 and -c2 option must be used with -M option.\n");
   fprintf(stderr,
           "         Without -c1 and -c2 option, a diagonal covariance can be obtained.\n");
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
       *dat, *pd, *cb, *icb, *logwgd, logb, *sum, *sum_m = NULL, **sum_v = NULL,
       diff, sum_w, ave_logp0 = 0.0, ave_logp1, change = MAXVALUE, tmp1, tmp2;
   int l, L = DEF_L, m, M = DEF_M, N, t, T = DEF_T, S =
       DEF_S, full = FULL, n1 = 0, i, j, k, Imin = DEF_IMIN, Imax =
       DEF_IMAX, *tindex, *cntcb, offset_row = 0, offset_col = 0,
       row = 0, col = 0;
   void cal_inv(double **cov, double **inv, const int L);
   float_list *top, *cur, *prev, *tmpf, *tmpff;
   Boolean block_full = FA, block_corr = FA, multiple_dim = FA, full_cov = FA;
   int cov_dim = 0, dim_list[1024];

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
            full = TR - full;
            full_cov = TR;
            break;
         case 'F':
            fgmm = getfp(*++argv, "rb");
            --argc;
            break;
            /* level 2 */
         case 'M':
            multiple_dim = TR;
            dim_list[0] = atoi(*++argv);
            i = 1;
            argc--;
            while ((**(argv + 1) != '\0') && isdigit(**(argv + 1))) {
               dim_list[i++] = atoi(*++argv);
               if (--argc <= 1) {
                  break;
               }
            }
            cov_dim = i;
            break;
         case 'c':
            if (strncmp("1", *(argv) + 2, 1) == 0) {
               block_corr = TR - block_corr;
            } else if (strncmp("2", *(argv) + 2, 1) == 0) {
               block_full = TR - block_full;
            }
            break;
         default:
            fprintf(stderr, "%s: Illegal option \"%s\".\n", cmnd, *argv);
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   if (multiple_dim == TR) {
      for (i = 0, j = 0; i < cov_dim; i++) {
         j += dim_list[i];
      }
      if (j != L) {
         fprintf(stderr,
                 "%s: block size must be coincided with dimention of vector\n",
                 cmnd);
         usage(1);
      }
      full = TR;
   } else {
      if (block_corr == TR || block_full == TR) {
         if (full_cov != TR) {
            fprintf(stderr,
                    "%s: -c1 and -c2 option must be specified with -M option!\n",
                    cmnd);
            usage(1);
         }
      }
   }

   /* -- Count number of input vectors and read -- */
   dat = dgetmem(L);
   top = prev = (float_list *) getmem(1, sizeof(float_list));
   top->f = fgetmem(L);
   T = 0;
   prev->next = NULL;
   while (freadf(dat, sizeof(*dat), L, fp) == L) {
      cur = (float_list *) getmem(1, sizeof(float_list));
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

   floor.gauss = NULL;
   sum_m = NULL;
   sum_v = NULL;
   if (full == TR) {
      floor.gauss = (Gauss *) getmem(1, sizeof(Gauss));
      floor.gauss[0].cov = (double **) getmem(L, sizeof(double *));
      for (l = 0; l < L; l++)
         floor.gauss[0].cov[l] = dgetmem(L);
      sum_m = dgetmem(L);
      sum_v = (double **) getmem(L, sizeof(double *));
   }

   logwgd = dgetmem(M);
   sum = dgetmem(M);

   /* Initialization of GMM parameters */
   alloc_GMM(&gmm, M, L, full);
   alloc_GMM(&tgmm, M, L, full);

   if (fgmm != NULL) {
      load_GMM(&gmm, fgmm);
      fprintf(stderr, "T = %d  L = %d  M = %d\n", T, L, M);
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
      if (full != TR) {
         for (t = 0, pd = dat; t < T; t++, pd += L)
            for (l = 0; l < L; l++) {
               diff = gmm.gauss[tindex[t]].mean[l] - pd[l];
               gmm.gauss[tindex[t]].var[l] += diff * diff;
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

         /* masking */
         if (multiple_dim == TR) {
            for (m = 0; m < M; m++) {
               offset_row = 0;
               for (row = 0; row < cov_dim; row++) {    /* row */
                  offset_col = 0;
                  for (col = 0; col <= row; col++) {    /* column */
                     for (k = offset_row; k < offset_row + dim_list[row]; k++) {
                        for (l = offset_col; l < offset_col + dim_list[col];
                             l++) {
                           if (dim_list[row] != dim_list[col]) {
                              gmm.gauss[m].cov[k][l] = 0.0;
                           } else {
                              if (block_full == FA && block_corr == FA) {
                                 if (row != col) {
                                    gmm.gauss[m].cov[k][l] = 0.0;
                                 } else {
                                    if (k - offset_row != l - offset_col) {
                                       gmm.gauss[m].cov[k][l] = 0.0;
                                    }
                                 }
                              } else if (block_full == FA && block_corr == TR) {
                                 if (k - offset_row != l - offset_col) {
                                    gmm.gauss[m].cov[k][l] = 0.0;
                                 }
                              } else if (block_full == TR && block_corr == FA) {
                                 if (row != col) {
                                    gmm.gauss[m].cov[k][l] = 0.0;
                                 }
                              }
                           }
                        }
                     }
                     offset_col += dim_list[col];
                  }
                  offset_row += dim_list[row];
               }
            }
         }
      }
   }                            /* end of initialization */

   /* EM training of GMM parameters */
   for (i = 0; (i <= Imax) && ((i <= Imin) || (fabs(change) > E)); i++) {
      fillz_GMM(&tgmm);
      fillz(sum, sizeof(double), M);

      if (full != TR) {
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
               fprintf(stderr, "ERROR : Can't caluculate covdet\n");
               exit(EXIT_FAILURE);
            }

            /* calculate inv */
            cal_inv(gmm.gauss[m].cov, gmm.gauss[m].inv, L);
         }
      }
      if (full == TR)
         fprintf(stderr, "%d cov can't caluculate covdet\n", n1);

      for (t = 0, ave_logp1 = 0.0, pd = dat; t < T; t++, pd += L) {
         for (m = 0, logb = LZERO; m < M; m++) {
            logwgd[m] = log_wgd(&gmm, m, L, pd);
            logb = log_add(logb, logwgd[m]);
         }
         ave_logp1 += logb;

         for (m = 0; m < M; m++) {
            tmp1 = exp(logwgd[m] - logb);
            sum[m] += tmp1;

            for (l = 0; l < L; l++) {
               tmp2 = tmp1 * pd[l];
               tgmm.gauss[m].mean[l] += tmp2;
               if (full != TR)
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

         if (multiple_dim == TR) {
            if (block_full == FA && block_corr == FA) {
               if (full_cov != TR) {    /* -f is not specified */
                  for (l = 0; l < L; l++) {
                     gmm.gauss[m].cov[l][l]
                         = tgmm.gauss[m].cov[l][l] / sum[m];
                  }
               } else {
                  for (l = 0; l < L; l++) {
                     for (j = 0; j <= l; j++) {
                        gmm.gauss[m].cov[l][j]
                            = tgmm.gauss[m].cov[l][j] / sum[m];
                     }
                  }
               }
            } else {
               /* for each block (lower triangle) */
               offset_row = 0;
               for (row = 0; row < cov_dim; row++) {    /* row block number */
                  offset_col = 0;
                  for (col = 0; col <= row; col++) {    /* column block number */
                     if (dim_list[row] == dim_list[col]) {      /* block is square */
                        if (block_full == FA && block_corr == TR) {
                           /* blockwise diagonal */
                           for (k = offset_row, l = offset_col;
                                k < offset_row + dim_list[row]; k++, l++) {
                              gmm.gauss[m].cov[k][l]
                                  = tgmm.gauss[m].cov[k][l]
                                  / sum[m];
                           }
                        } else {        /* block_full is TR */
                           for (k = offset_row; k < offset_row + dim_list[row];
                                k++) {
                              for (l = offset_col;
                                   l < offset_col + dim_list[col]; l++) {
                                 if (row == col) {
                                    if (l <= k) {
                                       gmm.gauss[m].cov[k][l]
                                           = tgmm.gauss[m].cov[k][l]
                                           / sum[m];
                                    }
                                 } else {
                                    if (block_corr == TR) {
                                       gmm.gauss[m].cov[k][l]
                                           = tgmm.gauss[m].cov[k][l]
                                           / sum[m];
                                    }
                                 }
                              }
                           }
                        }
                     }
                     offset_col += dim_list[col];
                  }
                  offset_row += dim_list[row];
               }
            }
         } else {
            if (full != TR) {
               for (l = 0; l < L; l++) {
                  gmm.gauss[m].var[l] = tgmm.gauss[m].var[l] / sum[m]
                      - gmm.gauss[m].mean[l] * gmm.gauss[m].mean[l];
                  if (gmm.gauss[m].var[l] < V) {
                     gmm.gauss[m].var[l] = V;
                  }
               }
            } else {
               for (l = 0; l < L; l++) {
                  for (j = 0; j <= l; j++) {
                     gmm.gauss[m].cov[l][j] = tgmm.gauss[m].cov[l][j] / sum[m];
                  }
               }
            }
         }
      }
   }

   /*  Output GMM parameters */
   save_GMM(&gmm, stdout);

   return (0);
}
