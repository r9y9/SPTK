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

/*********************************************************************************************
*                                                                                            *
*    Delta Calculation                                                                       *
*                                                                                            *
*                                                                   2008.6 H.Zen             *
*                                                                   2013.10 Akira Tamamori   *
*       usage:                                                                               *
*               delta [ options ] [ infile ] > stdout                                        *
*       options:                                                                             *
*               -m M                   : order of vector                              [24]   *
*               -l L                   : length of vector                             [m+1]  *
*               -d fn                  : filename of delta coefficients               [N/A]  *
*               -d coef [coef...]      : delta coefficients                           [N/A]  *
*               -r n w1 [w2]           : order and width of regression coefficients   [N/A]  *
*               -R n Wf1 Wb1 [Wf2 Wb2] : order and width of regression coefficients   [N/A]  *
*               -M magic               : magic number                                 [N/A]  *
*               -n N                   : order of regression polynomial               [N/A]  *
*               -e e                   : small value added to diagonal component      [0.0]  *
*                                        for calculating regression coefficients             *
*       infile:                                                                              *
*              static feature sequence                                                       *
*                      x_1(1), ..., x_1(L), x_2(1), ..., x_2(L), x_3(1), ...                 *
*       stdout:                                                                              *
*              static and dynamic feature sequence                                           *
*                      x_1(1), ..., x_1(L), \Delta x_1(1), ..., \Delta x_1(L), ...           *
*                                                                                            *
**********************************************************************************************/

static char *rcs_id = "$Id: delta.c,v 1.23 2014/12/11 08:30:33 uratec Exp $";


/*  Standard C Libraries  */
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
#define  LENG  25
#define  T     -1
#define MAGIC_FLAG FA
#define POLYNOMIAL_ORDER -1
#define POLY_FLAG FA
#define FLOOR 0.0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;

/* magic number */
Boolean magic_flag = MAGIC_FLAG;
double magic;

/* polynomial order */
Boolean poly_flag = POLY_FLAG;

/*  Other Definitions  */
#ifdef DOUBLE
typedef double real;
#else
typedef float real;
#endif

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - delta calculation\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m M                       : order of vector                 [%d]\n",
           LENG - 1);
   fprintf(stderr,
           "       -l L                       : length of vector                [m+1]\n");
   fprintf(stderr,
           "       -t T                       : number of input vectors         [EOF]\n");
   fprintf(stderr,
           "       -d coef [coef...]          : delta coefficients              [N/A]\n");
   fprintf(stderr,
           "       -r n t1 [t2]               : order and width of regression   [N/A]\n");
   fprintf(stderr, "                                    coefficients\n");
   fprintf(stderr,
           "       -R n Wf1 Wb1 [Wf2 Wb2] ... : order and width of regression   [N/A]\n");
   fprintf(stderr, "                                    coefficients\n");
   fprintf(stderr,
           "                                    Combining -M option, magic\n");
   fprintf(stderr,
           "                                    number is skipped during\n");
   fprintf(stderr,
           "                                    the delta calculation.\n");
   fprintf(stderr,
           "                                    Order n can be greater than 2.\n");
   fprintf(stderr,
           "       -M magic                   : magic number                    [N/A]\n");
   fprintf(stderr,
           "                                    valid only when -R option is\n");
   fprintf(stderr, "                                    specified.\n");
   fprintf(stderr,
           "       -n N                       : order of regression polynomial  [N/A]\n");
   fprintf(stderr,
           "                                    Order must be less than or\n");
   fprintf(stderr,
           "                                    equal to the max of (Wfn + Wbn).\n");
   fprintf(stderr,
           "       -e e                       : small value added to            [%g]\n",
           FLOOR);
   fprintf(stderr,
           "                                    diagonal component for  \n");
   fprintf(stderr,
           "                                    calculating regression coefficients \n");
   fprintf(stderr, "       -h                         : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       static feature vectors                                       [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       static and dynamic feature vectors\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

/* calculate regression polynomial coefficients */
void get_coef(double *input, double *output, int dw_num,
              int *position, int non_magic_frame, int total, int length,
              int *win_size_forward, int *win_size_backward, int poly_order,
              double floor)
{
   int i, j, k, l, t, d, ind = 0, index = 0, width = 0, input_val = 0,
       num_points = 0, num_order = 0, max_points = 0,
       max_degrees = 0, factorial = 1;
   double *poly_coef = NULL, *b = NULL, **p = NULL, *tmpvec = NULL,
       **inverse = NULL, **AA = NULL, **A = NULL;
   Boolean boundary_begin = FA, boundary_end = FA;

   max_points = win_size_forward[0] + win_size_backward[0] + 1;
   for (d = 1; d < dw_num - 1; d++) {
      if (max_points < win_size_forward[d] + win_size_backward[d] + 1) {
         max_points = win_size_forward[d] + win_size_backward[d] + 1;
      }
   }

   /* memory allocation */
   max_degrees = max_points - 1;
   AA = ddgetmem(max_degrees + 1, max_degrees + 1);
   A = ddgetmem(max_points, max_degrees + 1);
   inverse = ddgetmem(max_degrees + 1, max_degrees + 1);
   poly_coef = dgetmem(max_degrees + 1);
   b = dgetmem(max_points);
   p = ddgetmem(total, max_points + 1);
   tmpvec = dgetmem(max_degrees + 1);

   for (d = 0; d < dw_num - 1; d++) {
      if (magic_flag == TR) {
         /* fill magic number in output stream */
         for (t = 0; t < non_magic_frame; t++) {
            for (l = 0; l < length; l++) {
               if (d == 0) {
                  output[dw_num * length * t + l] = magic;
                  output[dw_num * length * t + length + l] = magic;
               } else {
                  output[dw_num * length * t + (d + 1) * length + l] = magic;
               }
            }
         }
      }

      /* decide number of points and order of regression polynomial from window size */
      num_points = win_size_forward[d] + win_size_backward[d] + 1;
      if (poly_flag == FA) {    /* order is not specified */
         num_order = num_points - 1;
      } else {
         num_order = poly_order;
      }

      /* calculate regression coefficients for each frame */
      for (t = 0; t < total; t++) {
         boundary_begin = boundary_end = FA;
         for (i = -win_size_backward[d]; i <= win_size_forward[d]; i++) {
            index = t + i;
            if (index < 0) {
               boundary_begin = TR;
               width = i;
            } else if (index >= total) {
               boundary_end = TR;
               width = i;
            } else {
               width = position[index] - position[t];
            }
            p[t][i + win_size_backward[d]] = width;
         }

         for (i = 0; i < num_points; i++) {
            for (j = 0; j < num_order + 1; j++) {
               A[i][j] = pow(p[t][i], j);
            }
         }
         for (i = 0; i < num_order + 1; i++) {
            for (j = 0; j < num_order + 1; j++) {
               AA[i][j] = 0.0;
               for (k = 0; k < num_points; k++) {
                  AA[i][j] += A[k][i] * A[k][j];
               }
            }
         }
         if (floor != 0.0) {
            for (i = 0; i < num_order + 1; i++) {
               AA[i][i] += floor;
            }
         }
         invert(AA, inverse, num_order + 1);

         /* for each dimension of feature vector */
         for (l = 0; l < length; l++) {
            for (i = -win_size_backward[d]; i <= win_size_forward[d]; i++) {
               index = t + i;
               if (index < 0) {
                  input_val = input[length * position[0] + l];
               } else if (index >= total) {
                  input_val = input[length * position[total - 1] + l];
               } else {
                  input_val = input[length * position[index] + l];
               }
               b[i + win_size_backward[d]] = input_val;
            }

            fillz(tmpvec, sizeof(*tmpvec), num_order + 1);
            for (i = 0; i < num_order + 1; i++) {
               for (j = 0; j < num_points; j++) {
                  tmpvec[i] += A[j][i] * b[j];
               }
            }
            fillz(poly_coef, sizeof(*poly_coef), num_order + 1);
            for (i = 0; i < num_order + 1; i++) {
               for (j = 0; j < num_order + 1; j++) {
                  poly_coef[i] += inverse[i][j] * tmpvec[j];
               }
            }

            if (d == 0) {
               /* output static */
               ind = dw_num * length * position[t] + l;
               output[ind] = input[length * position[t] + l];
               /* output delta */
               ind += length;
               if (boundary_begin == TR && win_size_backward[d] == 1) {
                  output[ind]
                      = (input[length * position[t + 1] + l]
                         - input[length * position[t] + l])
                      / (position[t + 1] - position[t]);
               } else if (boundary_end == TR && win_size_forward[d] == 1) {
                  output[ind]
                      = (input[length * position[t] + l]
                         - input[length * position[t - 1] + l])
                      / (position[t] - position[t - 1]);
               } else {
                  output[ind] = poly_coef[1];
               }
            } else {
               /* output delta-delta or higher order (greater than 2) dynamic feature */
               ind = dw_num * length * position[t] + (d + 1) * length + l;
               if (boundary_begin == TR && win_size_backward[d] == 1) {
                  output[ind] = 0.0;
               } else if (boundary_end == TR && win_size_forward[d] == 1) {
                  output[ind] = 0.0;
               } else {
                  /* calculate factorial */
                  for (i = 1, factorial = 1; i <= d + 1; i++) {
                     factorial *= i;
                  }
                  output[ind] = (double) factorial *poly_coef[d + 1];
               }
            }
         }
      }
   }
}

int main(int argc, char *argv[])
{
   FILE *fp = stdin, *fpc = NULL;
   char *coef = NULL;
   double floor = FLOOR, *x = NULL, *dx = NULL, **dw_coef = NULL;
   int i, j, l, d, t, tj, fsize, leng = LENG, total = T, win_buf = 0,
       poly_order = POLYNOMIAL_ORDER;
   int dw_num = 1, **dw_width = NULL, dw_calccoef = -1, dw_coeflen = 1,
       dw_leng = 1;
   char **dw_fn = (char **) getmem(argc, sizeof(char *));
   int non_magic_num, win_size_forward[2] = { 1, 1 }, win_size_backward[2] = {
   1, 1};
   float_list *top = NULL, *cur = NULL, *prev = NULL, *tmpf = NULL;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'd':
            if (dw_calccoef == 1 || dw_calccoef == 2) {
               fprintf(stderr,
                       "%s : Options '-r' and '-d' should not be defined simultaneously!\n",
                       cmnd);
               return (1);
            }
            dw_calccoef = 0;
            if (isfloat(*++argv)) {
               dw_coeflen = 0;
               for (i = 0; (i < argc - 1) && isfloat(argv[i]); i++) {
                  dw_coeflen += strlen(argv[i]) + 1;
               }
               dw_coeflen += 1;
               coef = dw_fn[dw_num] = getmem(dw_coeflen, sizeof(char));
               for (j = 0; j < i; j++) {
                  sprintf(coef, " %s", *argv);
                  coef += strlen(*argv) + 1;
                  if (j < i - 1) {
                     argv++;
                     argc--;
                  }
               }
            } else {
               dw_fn[dw_num] = *argv;
            }
            dw_num++;
            --argc;
            break;
         case 'r':
            if (dw_calccoef == 0 || dw_calccoef == 2) {
               fprintf(stderr,
                       "%s : Options '-r' and '-d' should not be defined simultaneously!\n",
                       cmnd);
               return (1);
            }
            dw_calccoef = 1;
            dw_coeflen = atoi(*++argv);
            --argc;
            if ((dw_coeflen != 1) && (dw_coeflen != 2)) {
               fprintf(stderr,
                       "%s : Number of delta parameter should be 1 or 2!\n",
                       cmnd);
               return (1);
            }
            if (argc <= 1) {
               fprintf(stderr,
                       "%s : Window size for delta parameter required!\n",
                       cmnd);
               return (1);
            }
            dw_fn[dw_num] = *++argv;
            dw_num++;
            --argc;
            if (dw_coeflen == 2) {
               if (argc <= 1) {
                  fprintf(stderr,
                          "%s : Window size for delta-delta parameter required!\n",
                          cmnd);
                  return (1);
               }
               dw_fn[dw_num] = *++argv;
               dw_num++;
               --argc;
            }
            break;
         case 'm':
            leng = atoi(*++argv) + 1;
            --argc;
            break;
         case 'l':
            leng = atoi(*++argv);
            --argc;
            break;
         case 'R':
            if (dw_calccoef == 0 || dw_calccoef == 1) {
               fprintf(stderr,
                       "%s : Options '-r', '-d' and '-R' should not be defined simultaneously!\n",
                       cmnd);
               return (1);
            }
            dw_calccoef = 2;
            dw_num = atoi(*++argv) + 1;
            --argc;
            if (argc <= 1) {
               fprintf(stderr,
                       "%s : Window size for delta-delta parameter required!\n",
                       cmnd);
               return (1);
            }

            for (d = 0; d < dw_num - 1; d++) {
               if (*(argv + 1) != NULL) {
                  if (sscanf(*++argv, "%d", &win_buf) < 1) {
                     fprintf(stderr,
                             "%s : Failure of reading window coefficient!\n",
                             cmnd);
                     return (1);
                  } else {
                     win_size_forward[d] = win_buf;
                     --argc;
                  }
               } else {
                  fprintf(stderr, "%s : Window coefficient is lacked!\n", cmnd);
                  return (1);
               }
               if (*(argv + 1) != NULL) {
                  if (sscanf(*++argv, "%d", &win_buf) < 1) {
                     fprintf(stderr,
                             "%s : Failure of reading window coefficient!\n",
                             cmnd);
                     return (1);
                  } else {
                     win_size_backward[d] = win_buf;
                     --argc;
                  }
               } else {
                  fprintf(stderr, "%s : Window coefficient is lacked!\n", cmnd);
                  return (1);
               }
            }
            break;
         case 'M':
            if (*(argv + 1) != NULL) {
               if (sscanf(*++argv, "%lf", &magic) < 1) {
                  fprintf(stderr, "%s : Failure of raeding magic number!\n",
                          cmnd);
                  return (1);
               } else {
                  magic_flag = TR;
                  --argc;
               }
            } else {
               fprintf(stderr, "%s : Magic number is lacked!\n", cmnd);
               return (1);
            }
            break;
         case 'n':
            if (*(argv + 1) != NULL) {
               if (sscanf(*++argv, "%d", &poly_order) < 1) {
                  fprintf(stderr,
                          "%s : Failure of reading regression polynomial order!\n",
                          cmnd);
                  return (1);
               } else {
                  poly_flag = TR;
                  --argc;
               }
            } else {
               fprintf(stderr,
                       "%s : Order of regression polynomial is lacked!\n",
                       cmnd);
               return (1);
            }
            break;
         case 'e':
            floor = atof(*++argv);
            if (floor < 0.0 || isdigit(**argv) == 0) {
               fprintf(stderr,
                       "%s : 'e' option must be specified with positive value.\n",
                       cmnd);
               usage(1);
            }
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");
   }

   /* check order of regression polynomial */
   if (dw_calccoef == 2) {
      if (poly_flag == TR) {
         for (d = 0; d < dw_num - 1; d++) {
            if (poly_order > win_size_forward[d] + win_size_backward[d]) {
               fprintf(stderr,
                       "%s : Order of regression polynomial is too large!\n",
                       cmnd);
               usage(1);
            } else if (poly_order <= 0) {
               fprintf(stderr,
                       "%s : Order of regression polynomial must be larger than 0!\n",
                       cmnd);
               usage(1);
            }
         }
      }
   }

   /* parse window files */
   /* memory allocation */
   if ((dw_width = (int **) getmem(dw_num, sizeof(int *))) == NULL) {
      fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
      exit(1);
   }
   for (i = 0; i < dw_num; i++)
      if ((dw_width[i] = (int *) getmem(2, sizeof(int))) == NULL) {
         fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
         exit(1);
      }
   if ((dw_coef = (double **) getmem(dw_num, sizeof(double *))) == NULL) {
      fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
      exit(1);
   }

   /* window for static parameter */
   dw_width[0][0] = dw_width[0][1] = 0;
   dw_coef[0] = dgetmem(1);
   dw_coef[0][0] = 1;

   /* set delta coefficients */
   if (dw_calccoef == 0) {
      for (i = 1; i < dw_num; i++) {
         if (dw_fn[i][0] == ' ') {
            fsize = str2darray(dw_fn[i], &(dw_coef[i]));
         } else {
            /* read from file */
            fpc = getfp(dw_fn[i], "rb");

            /* check the number of coefficients */
            fseek(fpc, 0L, 2);
            fsize = ftell(fpc) / sizeof(real);
            fseek(fpc, 0L, 0);

            /* read coefficients */
            dw_coef[i] = dgetmem(fsize);
            freadf(dw_coef[i], sizeof(**(dw_coef)), fsize, fpc);
         }

         /* set pointer */
         dw_leng = fsize / 2;
         dw_coef[i] += dw_leng;
         dw_width[i][0] = -dw_leng;
         dw_width[i][1] = dw_leng;
         if (fsize % 2 == 0)
            dw_width[i][1]--;
      }
   } else if (dw_calccoef == 1) {
      int a0, a1, a2;
      for (i = 1; i < dw_num; i++) {
         dw_leng = atoi(dw_fn[i]);
         if (dw_leng < 1) {
            fprintf(stderr,
                    "%s : Width for regression coefficient shuould be more than 1!\n",
                    cmnd);
            exit(1);
         }
         dw_width[i][0] = -dw_leng;
         dw_width[i][1] = dw_leng;
         dw_coef[i] = dgetmem(dw_leng * 2 + 1);
         dw_coef[i] += dw_leng;
      }

      dw_leng = atoi(dw_fn[1]);
      for (a1 = 0, j = -dw_leng; j <= dw_leng; a1 += j * j, j++);
      for (j = -dw_leng; j <= dw_leng; j++)
         dw_coef[1][j] = (double) j / (double) a1;

      if (dw_num > 2) {
         dw_leng = atoi(dw_fn[2]);
         for (a0 = a1 = a2 = 0, j = -dw_leng; j <= dw_leng;
              a0++, a1 += j * j, a2 += j * j * j * j, j++);
         for (j = -dw_leng; j <= dw_leng; j++)
            dw_coef[2][j] =
                2 * ((double) (a0 * j * j - a1)) /
                ((double) (a2 * a0 - a1 * a1));
      }
   }

   /* -- Count number of input vectors and read -- */
   x = dgetmem(leng);
   top = prev = (float_list *) getmem(1, sizeof(float_list));
   top->f = fgetmem(leng);
   total = 0;
   prev->next = NULL;
   while (freadf(x, sizeof(*x), leng, fp) == leng) {
      cur = (float_list *) getmem(1, sizeof(float_list));
      cur->f = fgetmem(leng);
      for (i = 0; i < leng; i++) {
         cur->f[i] = (float) x[i];
      }
      total++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }
   free(x);
   x = dgetmem(leng * total);
   dx = dgetmem(dw_num * leng * total);
   fillz(dx, sizeof(*x), dw_num * leng * total);
   for (i = 0, tmpf = top->next; tmpf != NULL; i++, tmpf = tmpf->next) {
      for (j = 0; j < leng; j++) {
         x[i * leng + j] = tmpf->f[j];
      }
   }

   if (dw_calccoef == 0 || dw_calccoef == 1) {
      /* calculate delta and delta-delta */
      for (t = 0; t < total; t++) {
         for (d = 0; d < dw_num; d++) {
            for (j = dw_width[d][0]; j <= dw_width[d][1]; j++) {
               tj = t + j;
               if (tj < 0)
                  tj = 0;
               if (!(tj < total))
                  tj = total - 1;
               for (l = 0; l < leng; l++)
                  dx[dw_num * leng * t + leng * d + l] +=
                      dw_coef[d][j] * x[leng * tj + l];
            }
         }
      }

      /* output static, delta, delta-delta */
      fwritef(dx, sizeof(*dx), dw_num * total * leng, stdout);

   } else if (dw_calccoef == 2) {
      int *position = (int *) getmem(total, sizeof(int));

      /* skip magic number */
      if (magic_flag == TR) {
         for (t = 0, non_magic_num = 0; t < total; t++) {
            for (l = 0; l < leng; l++) {
               if (x[leng * t + l] == magic) {
                  break;
               }
            }
            if (l == leng) {
               /* remember position of non-magic number */
               position[non_magic_num] = t;
               non_magic_num++;
            }
         }
      } else {
         for (t = 0; t < total; t++) {
            position[t] = t;
         }
         non_magic_num = total;
      }

      /* calculate delta and delta-delta */
      get_coef(x, dx, dw_num, position, total, non_magic_num, leng,
               win_size_forward, win_size_backward, poly_order, floor);

      /* output static, delta and delta-delta */
      fwritef(dx, sizeof(*dx), dw_num * total * leng, stdout);
   }

   return (0);
}
