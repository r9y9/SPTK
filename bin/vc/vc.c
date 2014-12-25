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

/******************************************************************************
*                                                                             *
*   GMM-based Voice Conversion                                                *
*                                                   2013.12 Akira Tamamori    *
*                                                                             *
*    usage:                                                                   *
*            vc [options] gmmfile [infile] > stdout                           *
*    options:                                                                 *
*            -l l               : dimension of source feature vector  [25]    *
*            -n n               : order of source feature vector      [l-1]   *
*            -L L               : dimension of target feature vector  [l]     *
*            -N N               : order of target feature vector      [L-1]   *
*            -m m               : number of mixture components of GMM [16]    *
*            -d fn              : filename of delta coefficients      [N/A]   *
*            -d coef [coef...]  : delta coefficients                  [N/A]   *
*            -r n w1 [w2]       : order and width of regression       [N/A]   *
*            -g fn              : filename of GV statistics           [N/A]   *
*            -e e               : small value added to                [0.0]   *
*                                 diagonal component of covariance            *
*    infile:                                                                  *
*            sequence of source static feature vectors                        *
*    gmmfile:                                                                 *
*            GMM trained from joint features of source and target             *
*    stdout:                                                                  *
*            sequence of converted static feature vectors                     *
*    notice:                                                                  *
*            When using -d option to specify filename of delta coefficients,  *
*            the number of coefficients must be odd.                          *
*                                                                             *
*******************************************************************************/

static char *rcs_id = "$Id: vc.c,v 1.10 2014/12/11 08:30:51 uratec Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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

/*  Default Values  */
#define DEF_L       25
#define DEF_M       16
#define FLOOR       0.0

/*  Command Name  */
char *cmnd;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

double *read_input(FILE * fp, size_t dim, size_t * length)
{
   size_t i, j;
   double *x = NULL, *input = NULL;
   float_list *top = NULL, *prev = NULL, *cur = NULL, *next = NULL, *tmp = NULL;

   input = dgetmem(dim);
   top = prev = (float_list *) getmem(1, sizeof(float_list));
   *length = 0;
   prev->next = NULL;

   while (freadf(input, sizeof(*input), dim, fp) == (int) dim) {
      cur = (float_list *) getmem(1, sizeof(float_list));
      cur->f = fgetmem(dim);
      for (i = 0; i < dim; i++) {
         cur->f[i] = (float) input[i];
      }
      (*length)++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }

   x = dgetmem(*length * dim);
   for (i = 0, tmp = top->next; tmp != NULL; i++, tmp = tmp->next) {
      for (j = 0; j < dim; j++) {
         x[i * dim + j] = tmp->f[j];
      }
   }

   for (tmp = top->next; tmp != NULL; tmp = next) {
      cur = tmp;
      next = tmp->next;
      free(cur->f);
      free(cur);
   }
   free(top);
   free(input);

   return (x);
}

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - GMM-based Voice Conversion\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] gmmfile [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -l l              : dimension of source feature vector         [%d]\n",
           DEF_L);
   fprintf(stderr,
           "       -n n              : order of source feature vector             [l-1]\n");
   fprintf(stderr,
           "       -L L              : dimension of target feature vector         [l]\n");
   fprintf(stderr,
           "       -N N              : order of target feature vector             [L-1]\n");
   fprintf(stderr,
           "       -m m              : number of mixture components of GMM        [%d]\n",
           DEF_M);
   fprintf(stderr,
           "       -d coef [coef...] : delta coefficients to calculate dynamic    [N/A]\n");
   fprintf(stderr, "                           feature\n");
   fprintf(stderr,
           "       -r n t1 [t2]      : order and width of regression coefficients [N/A]\n");
   fprintf(stderr, "                           for dynamic feature\n");
   fprintf(stderr,
           "       -g fn             : filename of GV statistics                  [N/A]\n");
   fprintf(stderr,
           "                           fn must contain mean vector and diagonal\n");
   fprintf(stderr,
           "                           components of covariance matrix of GV\n");
   fprintf(stderr,
           "       -e e              : small value added to                       [%g]\n",
           FLOOR);
   fprintf(stderr,
           "                           diagonal component of covariance\n");
   fprintf(stderr, "       -h                : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       sequence of source static feature vectors (%s)              [stdin]\n",
           FORMAT);
   fprintf(stderr, "  gmmfile:\n");
   fprintf(stderr,
           "       GMM trained from joint features of source and target (%s)\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr,
           "       sequence of converted target static feature vectors (%s)\n",
           FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr,
           "       When using -d option to specify filename of delta coefficients, \n");
   fprintf(stderr, "       the number of coefficients must be odd. \n");
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
   size_t i, source_vlen = DEF_L, target_vlen = 0, len_total = 0, num_mix =
       DEF_M, total_frame = 0;
   char *coef = NULL, **dw_fn = (char **) getmem(argc, sizeof(*(dw_fn)));
   int j, k, dw_num = 1, dw_calccoef = -1, dw_coeflen = 1, win_max_width = 0;
   double floor = FLOOR;
   double *source = NULL, *target = NULL, *gv_mean = NULL, *gv_vari = NULL;
   FILE *fp = stdin, *fgmm = NULL, *fgv = NULL;
   Boolean full = TR;
   GMM gmm;
   DELTAWINDOW window;

   memset(dw_fn, 0, argc * sizeof(*dw_fn));

   if ((cmnd = strrchr(argv[0], '/')) == NULL) {
      cmnd = argv[0];
   } else {
      cmnd++;
   }
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            source_vlen = atoi(*++argv);
            --argc;
            break;
         case 'n':
            source_vlen = atoi(*++argv) + 1;
            --argc;
            break;
         case 'L':
            target_vlen = atoi(*++argv);
            --argc;
            break;
         case 'N':
            target_vlen = atoi(*++argv) + 1;
            --argc;
            break;
         case 'm':
            num_mix = atoi(*++argv);
            --argc;
            break;
         case 'd':
            if (dw_calccoef == 1 || dw_calccoef == 2) {
               fprintf(stderr,
                       "%s : Options '-r' and '-d' should not be defined simultaneously!\n",
                       cmnd);
               usage(EXIT_FAILURE);
            }
            dw_calccoef = 0;
            if (isfloat(*++argv)) {
               dw_coeflen = 0;
               for (k = 0; (k < argc - 1) && isfloat(argv[k]); k++) {
                  dw_coeflen += strlen(argv[k]) + 1;
               }
               dw_coeflen += 1;
               coef = dw_fn[dw_num] = getmem(dw_coeflen, sizeof(*coef));
               for (j = 0; j < k; j++) {
                  sprintf(coef, " %s", *argv);
                  coef += strlen(*argv) + 1;
                  if (j < k - 1) {
                     argv++;
                     argc--;
                  }
               }
            } else {
               dw_fn[dw_num] = getmem(strlen(*argv) + 1, sizeof(**dw_fn));
               strncpy(dw_fn[dw_num], *argv, strlen(*argv) + 1);
            }
            dw_num++;
            --argc;
            break;
         case 'r':
            if (dw_calccoef == 0 || dw_calccoef == 2) {
               fprintf(stderr,
                       "%s : Options '-r' and '-d' should not be defined simultaneously!\n",
                       cmnd);
               usage(EXIT_FAILURE);
            }
            dw_calccoef = 1;
            dw_coeflen = atoi(*++argv);
            --argc;
            if ((dw_coeflen != 1) && (dw_coeflen != 2)) {
               fprintf(stderr,
                       "%s : Number of delta parameter should be 1 or 2!\n",
                       cmnd);
               usage(EXIT_FAILURE);
            }
            if (argc <= 1) {
               fprintf(stderr,
                       "%s : Window size for delta parameter required!\n",
                       cmnd);
               usage(EXIT_FAILURE);
            }
            dw_fn[dw_num] = getmem(strlen(*++argv) + 1, sizeof(**dw_fn));
            strncpy(dw_fn[dw_num], *argv, strlen(*argv) + 1);
            dw_num++;
            --argc;
            if (dw_coeflen == 2) {
               if (argc <= 1) {
                  fprintf(stderr,
                          "%s : Window size for delta-delta parameter required!\n",
                          cmnd);
                  usage(EXIT_FAILURE);
               }
               dw_fn[dw_num] = getmem(strlen(*++argv) + 1, sizeof(**dw_fn));
               strncpy(dw_fn[dw_num], *argv, strlen(*argv) + 1);
               dw_num++;
               --argc;
            }
            break;
         case 'g':
            fgv = getfp(*++argv, "rb");
            --argc;
            break;
         case 'e':
            floor = atof(*++argv);
            if (floor < 0.0 || isdigit(**argv) == 0) {
               fprintf(stderr,
                       "%s : '-e' option must be specified with positive value.\n",
                       cmnd);
               usage(1);
            }
            --argc;
            break;
         case 'h':
            usage(EXIT_SUCCESS);
         default:
            fprintf(stderr, "%s: Illegal option %s.\n", cmnd, *argv);
            usage(EXIT_FAILURE);
         }
      } else if (fgmm == NULL) {
         fgmm = getfp(*argv, "rb");
      } else {
         fp = getfp(*argv, "rb");
      }
   }

   if (fgmm == NULL) {
      fprintf(stderr, "%s: GMM file must be specified!\n", cmnd);
      usage(EXIT_FAILURE);
   }

   /* set dimensionarity of joint vector */
   if (target_vlen == 0) {
      target_vlen = source_vlen;
   }
   len_total = (source_vlen + target_vlen) * dw_num;

   /* read sequence of source feature vectors */
   source = read_input(fp, source_vlen, &total_frame);
   fclose(fp);
   target = dgetmem(target_vlen * total_frame);

   /* load GMM parameters */
   alloc_GMM(&gmm, num_mix, len_total, full);
   load_GMM(&gmm, fgmm);
   prepareCovInv_GMM(&gmm);
   prepareGconst_GMM(&gmm);
   fclose(fgmm);

   /* flooring for diagonal component of covariance */
   if (floor != 0.0) {
      for (i = 0; i < num_mix; i++) {
         for (j = 0; j < (int) len_total; j++) {
            gmm.gauss[i].cov[j][j] += floor;
         }
      }
   }

   /* load GV parameters */
   if (fgv != NULL) {
      gv_mean = dgetmem(target_vlen);
      gv_vari = dgetmem(target_vlen);
      freadf(gv_mean, sizeof(*gv_mean), target_vlen, fgv);
      freadf(gv_vari, sizeof(*gv_vari), target_vlen, fgv);
      fclose(fgv);
   }

   /* set window parameters */
   window.win_size = dw_num;
   window.win_l_width =
       (int *) getmem(window.win_size, sizeof(*(window.win_l_width)));
   window.win_r_width =
       (int *) getmem(window.win_size, sizeof(*(window.win_r_width)));
   window.win_coefficient =
       (double **) getmem(window.win_size, sizeof(*(window.win_coefficient)));
   window.win_l_width[0] = 0;
   window.win_r_width[0] = 0;
   window.win_coefficient[0] = dgetmem(1);
   window.win_coefficient[0][0] = 1.0;
   if (dw_calccoef == 0) {
      int fsize, dw_leng;
      FILE *fpc = NULL;
      for (i = 1; i < window.win_size; i++) {
         if (dw_fn[i][0] == ' ') {
            fsize = str2darray(dw_fn[i], &(window.win_coefficient[i]));
         } else {
            /* read from file */
            fpc = getfp(dw_fn[i], "rb");

            /* check the number of coefficients */
            fseek(fpc, 0L, SEEK_END);
            fsize = ftell(fpc) / sizeof(float);
            fseek(fpc, 0L, SEEK_SET);
            if (fsize % 2 == 0) {
               fprintf(stderr,
                       "%s : number of delta coefficients must be odd!\n",
                       cmnd);
               usage(EXIT_FAILURE);
            }

            /* read coefficients */
            window.win_coefficient[i] = dgetmem(fsize);
            freadf(window.win_coefficient[i],
                   sizeof(*(window.win_coefficient[i])), fsize, fpc);
         }

         /* set pointer */
         dw_leng = fsize / 2;
         window.win_coefficient[i] += dw_leng;
         window.win_l_width[i] = -dw_leng;
         window.win_r_width[i] = dw_leng;
      }
      fclose(fpc);
   } else if (dw_calccoef == 1) {
      int a0, a1, a2, dw_leng;
      for (i = 1; i < window.win_size; i++) {
         dw_leng = atoi(dw_fn[i]);
         if (dw_leng < 1) {
            fprintf(stderr,
                    "%s : Width for regression coefficient shuould be more than 1!\n",
                    cmnd);
            usage(EXIT_FAILURE);
         }
         window.win_l_width[i] = -dw_leng;
         window.win_r_width[i] = dw_leng;
         window.win_coefficient[i] = dgetmem(dw_leng * 2 + 1);
         window.win_coefficient[i] += dw_leng;
      }
      dw_leng = atoi(dw_fn[1]);
      for (a1 = 0, j = -dw_leng; j <= dw_leng; a1 += j * j, j++);
      for (j = -dw_leng; j <= dw_leng; j++) {
         window.win_coefficient[1][j] = (double) j / (double) a1;
      }

      if (window.win_size > 2) {
         dw_leng = atoi(dw_fn[2]);
         for (a0 = a1 = a2 = 0, j = -dw_leng; j <= dw_leng;
              a0++, a1 += j * j, a2 += j * j * j * j, j++);
         for (j = -dw_leng; j <= dw_leng; j++) {
            window.win_coefficient[2][j]
                = 2 * ((double) (a0 * j * j - a1)) /
                ((double) (a2 * a0 - a1 * a1));
         }
      }
   }
   win_max_width = window.win_r_width[0];       /* width of static window is 0 */
   for (i = 1; i < window.win_size; i++) {
      if (win_max_width < window.win_r_width[i]) {
         win_max_width = window.win_r_width[i];
      }
      if (win_max_width < -window.win_l_width[i]) {
         win_max_width = -window.win_l_width[i];
      }
   }
   window.win_max_width = win_max_width;

   /* perform conversion */
   vc(&gmm, &window, total_frame, source_vlen, target_vlen, gv_mean, gv_vari,
      source, target);

   /* output sequence of converted target static feature vectors */
   fwritef(target, sizeof(*target), target_vlen * total_frame, stdout);

   /* release memory */
   free(source);
   free(target);
   free(gv_mean);
   free(gv_vari);
   free_GMM(&gmm);
   for (i = 0; i < window.win_size; i++) {
      if (dw_fn[i]) {
         free(dw_fn[i]);
      }
      free(window.win_coefficient[i] + window.win_l_width[i]);
   }
   free(dw_fn);
   free(window.win_l_width);
   free(window.win_r_width);
   free(window.win_coefficient);

   return (0);
}
