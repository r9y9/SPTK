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
*                                                                          *
*    Dynamic Time Warping                                                  *
*                                                                          *
*                                      2011.12 Akira Tamamori              *
*       usage:                                                             *
*               dtw [ options ] reffile [ infile ] > stdout                *
*       options:                                                           *
*               -m M      : order of vector                        [24]    *
*               -l L      : dimension of vector                    [m+1]   *
*               -t T      : number of test vectors                 [N/A]   *
*               -r R      : number of reference vectors            [N/A]   *
*               -n N      : type of norm used for calculation      [2]     *
*                           of local distance                              *
*                             1 : L1-norm                                  *
*                             2 : L2-norm                                  *
*               -p P      : local path constraint                  [5]     *
*               -s sfile  : output score of dynamic time warping   [FALSE] *
*               -v vfile  : output concatenated test/reference     [FALSE] *
*                           data sequence along the Vitebi path            *
*       infile:                                                            *
*              test vector sequence                                        *
*                  x_1(1), ..., x_1(L), x_2(1), ..., x_2(L), ...           *
*       reffile:                                                           *
*              reference vector sequence                                   *
*                  y_1(1), ..., y_1(L), y_2(1), ..., y_2(L), ...           *
*       stdout:                                                            *
*              concatenated test/reference vector sequence                 *
*              along the Vitebi path                                       *
*                  x_1(1), ..., x_1(L), y_1(1), ..., y_1(L), ...           *
*                                                                          *
***************************************************************************/

static char *rcs_id = "$Id: dtw.c,v 1.8 2013/12/16 09:01:55 mataki Exp $";

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
#define  LENG  24

/*  Command Name  */
char *cmnd;

#define PATH_NG FA
#define PATH_OK TR

enum Norm { L1 = 1, L2 };
enum PATH { I = 1, II, III, IV, V, VI, VII };
enum Allowable_Path { P1 = 1, P2, P3, P4, P5, P6, P7, P8, P9 };

typedef struct _DTW_Cell {
   double local;                /* local cost */
   double global;               /* global cost */
   int backptr[2];              /* back pointer for Viterbi path */
   Boolean is_region;
   enum Allowable_Path allow_path;
} DTW_Cell;

typedef struct _DATA {
   double *input;
   int total;                   /* total number of vectors */
   int dim;                     /* dimension of input vector */
   int *viterbi;                /* Viterbi path */
} DATA;

typedef struct _WEIGHT {
   double *val;                 /* weight on the local path */
} WEIGHT;

typedef struct _DTW_Table {
   DTW_Cell **cell;
   DATA data[2];                /* two comparative data */
   int vit_leng;                /* length of Viterbi path */
   enum PATH path;              /* type of local constraint */
   enum Norm norm;              /* type of norm for local cost */
   WEIGHT weight;
} DTW_Table;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

static int round_up(double dat)
{
   return (int) (dat + 0.5);
}

double *read_input(FILE * fp, int dim, int *length)
{
   int i, j;
   double *x = NULL, *input;
   float_list *top, *prev, *cur, *next, *tmp;

   input = dgetmem(dim);
   top = prev = (float_list *) malloc(sizeof(float_list));
   *length = 0;
   prev->next = NULL;

   while (freadf(input, sizeof(*input), dim, fp) == dim) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = (float *) malloc(sizeof(float) * dim);
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
      cur->f = NULL;
      cur = NULL;
   }
   free(top);
   free(input);
   top = NULL;
   input = NULL;

   return (x);
}

DTW_Cell **malloc_DTW_Cell(int size1, int size2)
{
   DTW_Cell **tmpcell, *tmpcell2;
   int i, j;
   void usage(int status);

   tmpcell = (DTW_Cell **) malloc(sizeof(DTW_Cell *) * size1);
   if (tmpcell == NULL) {
      fprintf(stderr, "ERROR: Can't allocate memory !\n");
      usage(EXIT_FAILURE);
   }

   tmpcell2 = (DTW_Cell *) malloc(sizeof(DTW_Cell) * size1 * size2);
   if (tmpcell2 == NULL) {
      fprintf(stderr, "ERROR: Can't allocate memory !\n");
      usage(EXIT_FAILURE);
   }

   for (i = 0, j = 0; i < size1; i++, j += size2) {
      tmpcell[i] = tmpcell2 + j;
   }

   return (tmpcell);
}

void init_dtw(DTW_Table * table, int leng, double *input1, double *input2,
              int total1, int total2, enum PATH path, enum Norm norm)
{

   int i, size[2] = { total1, total2 };
   void usage(int status);

   if (path == III || path == IV) {
      if (total2 > total1) {
         fprintf(stderr, "Can't perform DTW !\n"
                 "The number of the reference vectors (= %d) must be less than "
                 "the number of the test vectors (= %d). \n", total2, total1);
         usage(EXIT_FAILURE);
      }
   } else if (path == V || path == VI || path == VII) {
      if (total1 / 2 >= total2) {
         fprintf(stderr, "Can't perform DTW !\n"
                 "The number of the test vectors (= %d) must be less than "
                 "the twice of the reference vectors (= 2 * %d = %d). \n",
                 total1, total2, 2 * total2);
         usage(EXIT_FAILURE);
      } else if (total2 / 2 >= total1) {
         fprintf(stderr, "Can't perform DTW !\n"
                 "The number of the reference vectors (= %d) must be less than "
                 "the twice of the test vectors (= 2 * %d = %d). \n",
                 total2, total1, 2 * total1);
         usage(EXIT_FAILURE);
      }
   }

   table->cell = (DTW_Cell **) malloc_DTW_Cell(size[0], size[1]);

   table->data[0].input = input1;
   table->data[1].input = input2;
   for (i = 0; i < 2; i++) {
      table->data[i].total = size[i];
      table->data[i].dim = leng;
      table->data[i].viterbi =
          (int *) malloc(sizeof(int) * (size[0] + size[1]));
      if (table->data[i].viterbi == NULL) {
         fprintf(stderr, "ERROR: Can't allocate memory at init_dtw() !\n");
         usage(EXIT_FAILURE);
      }
   }

   table->path = path;

   if (norm != L1 && norm != L2) {
      fprintf(stderr, "%s : type of norm must be %d or %d!\n", cmnd, L1, L2);
      usage(EXIT_FAILURE);
   }
   table->norm = norm;

   switch (path) {
   case I:
      table->weight.val = dgetmem(2);
      table->weight.val[0] = 1.0;
      table->weight.val[1] = 1.0;
      break;
   case II:
      table->weight.val = dgetmem(3);
      table->weight.val[0] = 1.0;
      table->weight.val[1] = 2.0;
      table->weight.val[2] = 1.0;
      break;
   case III:
      table->weight.val = dgetmem(2);
      table->weight.val[0] = 1.0;
      table->weight.val[1] = 2.0;
      break;
   case IV:
      table->weight.val = dgetmem(3);
      table->weight.val[0] = 1.0;
      table->weight.val[1] = 2.0;
      table->weight.val[2] = 3.0;
      break;
   case V:
      table->weight.val = dgetmem(5);
      table->weight.val[0] = 2.0;
      table->weight.val[1] = 1.0;
      table->weight.val[2] = 2.0;
      table->weight.val[3] = 2.0;
      table->weight.val[4] = 1.0;
      break;
   case VI:
      table->weight.val = dgetmem(3);
      table->weight.val[0] = 3.0;
      table->weight.val[1] = 2.0;
      table->weight.val[2] = 3.0;
      break;
   case VII:
      table->weight.val = dgetmem(6);
      table->weight.val[0] = 1.0;
      table->weight.val[1] = 1.0;
      table->weight.val[2] = 1.0;
      table->weight.val[3] = 1.0;
      table->weight.val[4] = 1.0;
      table->weight.val[5] = 1.0;
      break;
   default:
      break;
   }
}

void check_enabled_region_type_1(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   for (j = 0; j < Ty; j++) {
      for (i = 0; i < Tx; i++) {
         table->cell[i][j].is_region = PATH_OK;
      }
   }
}

void check_enabled_region_type_2(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   for (j = 0; j < Ty; j++) {
      for (i = 0; i < Tx; i++) {
         table->cell[i][j].is_region = PATH_OK;
      }
   }
}

void check_enabled_region_type_3(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total,
       range = Tx - Ty;
   DTW_Cell **cell = table->cell;

   cell[0][0].is_region = PATH_OK;

   for (j = 1; j < Ty; j++) {
      cell[0][j].is_region = PATH_NG;
   }
   for (i = 1; i < Tx; i++) {
      cell[i][0].is_region = PATH_NG;
   }
   for (i = 1; i <= range; i++) {
      cell[i][0].is_region = PATH_OK;
   }

   for (j = 1; j < Ty; j++) {
      for (i = 1; i < j; i++) {
         cell[i][j].is_region = PATH_NG;
      }
      for (i = j; i <= range + j; i++) {
         cell[i][j].is_region = PATH_OK;
      }
      for (; i < Tx; i++) {
         cell[i][j].is_region = PATH_NG;
      }
   }

   for (j = 1; j < Ty; j++) {
      for (i = 1; i < Tx; i++) {
         if (cell[i][j].is_region == PATH_OK) {
            if (cell[i - 1][j].is_region == PATH_OK &&
                cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P1;
            } else if (cell[i - 1][j].is_region == PATH_OK) {
               cell[i][j].allow_path = P2;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P3;
            }
         }
      }
   }
}

void check_enabled_region_type_4(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   DTW_Cell **cell = table->cell;

   cell[0][0].is_region = PATH_OK;

   for (j = 1; j < Ty; j++) {
      cell[0][j].is_region = PATH_NG;
   }

   for (i = 1; i < Tx - Ty / 2; i++) {
      cell[i][0].is_region = PATH_OK;
   }
   for (; i < Tx; i++) {
      cell[i][0].is_region = PATH_NG;
   }
   for (i = 1; i < Tx - Ty / 2 + 1; i++) {
      cell[i][1].is_region = PATH_OK;
   }
   for (; i < Tx; i++) {
      cell[i][1].is_region = PATH_NG;
   }

   for (j = 2; j < Ty; j++) {
      for (i = 0; i < round_up((double) j / 2); i++) {
         cell[i][j].is_region = PATH_NG;
      }
      for (; i < Tx - Ty / 2 + round_up((double) j / 2); i++) {
         cell[i][j].is_region = PATH_OK;
      }
      for (; i < Tx; i++) {
         cell[i][j].is_region = PATH_NG;
      }
   }
   cell[Tx - 1][Ty - 1].is_region = PATH_OK;

   for (j = 2; j < Ty; j++) {
      for (i = 2; i < Tx; i++) {
         if (cell[i][j].is_region == PATH_OK) {
            if (cell[i - 1][j].is_region == PATH_OK &&
                cell[i - 1][j - 1].is_region == PATH_OK &&
                cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P1;
            } else if (cell[i - 1][j].is_region == PATH_OK &&
                       cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P2;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P3;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P4;
            } else if (cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P5;
            }
         }
      }
   }
}

void check_enabled_region_type_5(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   DTW_Cell **cell = table->cell;

   cell[0][0].is_region = PATH_OK;

   for (j = 1; j < Ty; j++) {
      for (i = 1; i < Tx; i++) {
         cell[i][j].is_region = PATH_OK;
      }
   }

   for (i = 1; i < Tx; i++) {
      cell[i][0].is_region = PATH_NG;
   }

   for (j = 1; j < Ty; j++) {
      cell[0][j].is_region = PATH_NG;
   }

   for (j = 1; j < Ty - 2; j++) {
      for (i = 2 * j + 1; i < Tx; i++) {
         cell[i][j].is_region = PATH_NG;
      }
   }
   for (i = 1; i < Tx - 2; i++) {
      for (j = 2 * i + 1; j < Ty; j++) {
         cell[i][j].is_region = PATH_NG;
      }
   }

   /* Backward pruning */
   for (j = Ty - 1; j > 0; j--) {
      for (i = 2 * (j - Ty) + Tx; i > 0; i--) {
         cell[i][j].is_region = PATH_NG;
      }
   }
   for (i = Tx - 1; i > 0; i--) {
      for (j = 2 * (i - Tx) + Ty; j > 0; j--) {
         cell[i][j].is_region = PATH_NG;
      }
   }

   for (j = 2; j < Ty - 1; j++) {
      for (i = 2; i < Tx - 1; i++) {
         if (cell[i][j].is_region == PATH_OK) {
            if (cell[i - 2][j - 1].is_region == PATH_OK &&
                cell[i - 1][j - 1].is_region == PATH_OK &&
                cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P1;
            } else if (cell[i - 2][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P2;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P3;
            } else if (cell[i - 2][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P4;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P5;
            } else if (cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P6;
            }
         }
      }
   }
}

void check_enabled_region_type_6(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   DTW_Cell **cell = table->cell;

   cell[0][0].is_region = PATH_OK;

   for (j = 1; j < Ty; j++) {
      for (i = 1; i < Tx; i++) {
         cell[i][j].is_region = PATH_OK;
      }
   }

   for (i = 1; i < Tx; i++) {
      cell[i][0].is_region = PATH_NG;
   }

   for (j = 1; j < Ty; j++) {
      cell[0][j].is_region = PATH_NG;
   }

   for (j = 1; j < Ty - 2; j++) {
      for (i = 2 * j + 1; i < Tx; i++) {
         cell[i][j].is_region = PATH_NG;
      }
   }
   for (i = 1; i < Tx - 2; i++) {
      for (j = 2 * i + 1; j < Ty; j++) {
         cell[i][j].is_region = PATH_NG;
      }
   }

   /* Backward pruning */
   for (j = Ty - 1; j > 0; j--) {
      for (i = Tx - 1 + 2 * (j - Ty) + 1; i > 0; i--) {
         cell[i][j].is_region = PATH_NG;
      }
   }
   for (i = Tx - 1; i > 0; i--) {
      for (j = Ty - 1 + 2 * (i - Tx) + 1; j > 0; j--) {
         cell[i][j].is_region = PATH_NG;
      }
   }

   for (j = 2; j < Ty; j++) {
      for (i = 2; i < Tx; i++) {
         if (cell[i][j].is_region == PATH_OK) {
            if (cell[i - 2][j - 1].is_region == PATH_OK &&
                cell[i - 1][j - 1].is_region == PATH_OK &&
                cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P1;
            } else if (cell[i - 2][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P2;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P3;
            } else if (cell[i - 2][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P4;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P5;
            } else if (cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P6;
            }
         }
      }
   }
}

void check_enabled_region_type_7(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   DTW_Cell **cell = table->cell;

   cell[0][0].is_region = PATH_OK;

   for (j = 1; j < Ty; j++) {
      for (i = 1; i < Tx; i++) {
         cell[i][j].is_region = PATH_OK;
      }
   }

   for (i = 1; i < Tx; i++) {
      cell[i][0].is_region = PATH_NG;
   }

   for (j = 1; j < Ty; j++) {
      cell[0][j].is_region = PATH_NG;
   }

   for (j = 1; j < Ty - 2; j++) {
      for (i = 2 * j + 1; i < Tx; i++) {
         cell[i][j].is_region = PATH_NG;
      }
   }
   for (i = 1; i < Tx - 2; i++) {
      for (j = 2 * i + 1; j < Ty; j++) {
         cell[i][j].is_region = PATH_NG;
      }
   }

   /* Backward pruning */
   for (j = Ty - 1; j > 0; j--) {
      for (i = Tx - 1 + 2 * (j - Ty) + 1; i > 0; i--) {
         cell[i][j].is_region = PATH_NG;
      }
   }
   for (i = Tx - 1; i > 0; i--) {
      for (j = Ty - 1 + 2 * (i - Tx) + 1; j > 0; j--) {
         cell[i][j].is_region = PATH_NG;
      }
   }

   for (j = 2; j < Ty; j++) {
      for (i = 2; i < Tx; i++) {
         if (cell[i][j].is_region == PATH_OK) {
            if (cell[i - 2][j - 1].is_region == PATH_OK &&
                cell[i - 2][j - 2].is_region == PATH_OK &&
                cell[i - 1][j - 1].is_region == PATH_OK &&
                cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P1;
            } else if (cell[i - 2][j - 2].is_region == PATH_OK &&
                       cell[i - 1][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P2;
            } else if (cell[i - 2][j - 1].is_region == PATH_OK &&
                       cell[i - 2][j - 2].is_region == PATH_OK &&
                       cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P3;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P4;
            } else if (cell[i - 2][j - 2].is_region == PATH_OK &&
                       cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P5;
            } else if (cell[i - 2][j - 1].is_region == PATH_OK &&
                       cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P6;
            } else if (cell[i - 2][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P7;
            } else if (cell[i - 1][j - 1].is_region == PATH_OK) {
               cell[i][j].allow_path = P8;
            } else if (cell[i - 1][j - 2].is_region == PATH_OK) {
               cell[i][j].allow_path = P9;
            }
         }
      }
   }
}

void recursive_calc_type_1(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   double local, path1, path2;
   DTW_Cell **cell = table->cell;
   WEIGHT weight = table->weight;

   for (i = 1; i < Tx; i++) {
      cell[i][0].global =
          cell[i - 1][0].global +weight.val[0] * cell[i][0].local;
      cell[i][0].backptr[0] = i - 1;
      cell[i][0].backptr[1] = 0;
   }
   for (j = 1; j < Ty; j++) {
      cell[0][j].global =
          cell[0][j - 1].global +weight.val[1] * cell[0][j].local;
      cell[0][j].backptr[0] = 0;
      cell[0][j].backptr[1] = j - 1;
   }
   for (i = 1; i < Tx; i++) {
      for (j = 1; j < Ty; j++) {
         local = cell[i][j].local;
         path1 = cell[i - 1][j].global +weight.val[0] * local;
         path2 = cell[i][j - 1].global +weight.val[1] * local;
         if (path1 < path2) {
            cell[i][j].global = path1;
            cell[i][j].backptr[0] = i - 1;
            cell[i][j].backptr[1] = j;
         } else {
            cell[i][j].global = path2;
            cell[i][j].backptr[0] = i;
            cell[i][j].backptr[1] = j - 1;
         }
      }
   }
   cell[Tx - 1][Ty - 1].global /=(Tx + Ty);
}

void recursive_calc_type_2(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   double local, min, path1, path2, path3;
   DTW_Cell **cell = table->cell;
   WEIGHT weight = table->weight;

   for (i = 1; i < Tx; i++) {
      if (cell[i][0].is_region == PATH_OK) {
         cell[i][0].global =
             cell[i - 1][0].global +weight.val[0] * cell[i][0].local;
         cell[i][0].backptr[0] = i - 1;
         cell[i][0].backptr[1] = 0;
      }
   }
   for (j = 1; j < Ty; j++) {
      cell[0][j].global =
          cell[0][j - 1].global +weight.val[2] * cell[0][j].local;
      cell[0][j].backptr[0] = 0;
      cell[0][j].backptr[1] = j - 1;
   }
   for (j = 1; j < Ty; j++) {
      for (i = 1; i < Tx; i++) {
         local = cell[i][j].local;
         path1 = cell[i - 1][j].global +weight.val[0] * local;
         path2 = cell[i - 1][j - 1].global +weight.val[1] * local;
         path3 = cell[i][j - 1].global +weight.val[2] * local;
         cell[i][j].backptr[0] = i - 1;
         cell[i][j].backptr[1] = j;
         min = path1;
         if (min >= path2) {
            min = path2;
            cell[i][j].backptr[0] = i - 1;
            cell[i][j].backptr[1] = j - 1;
         }
         if (min >= path3) {
            min = path3;
            cell[i][j].backptr[0] = i;
            cell[i][j].backptr[1] = j - 1;
         }
         cell[i][j].global = min;
      }
   }
   cell[Tx - 1][Ty - 1].global /=(Tx + Ty);
}

void recursive_calc_type_3(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   double local, min = 0.0, path1, path2;
   DTW_Cell **cell = table->cell;
   WEIGHT weight = table->weight;

   for (i = 1; i < Tx; i++) {
      if (cell[i][0].is_region == PATH_OK) {
         cell[i][0].global =
             cell[i - 1][0].global +weight.val[0] * cell[i][0].local;
         cell[i][0].backptr[0] = i - 1;
         cell[i][0].backptr[1] = 0;
      }
   }
   for (j = 1; j < Ty; j++) {
      for (i = 1; i < Tx; i++) {
         local = cell[i][j].local;
         if (cell[i][j].is_region == PATH_OK) {
            path1 = cell[i - 1][j].global +weight.val[0] * local;
            path2 = cell[i - 1][j - 1].global +weight.val[1] * local;

            switch (cell[i][j].allow_path) {
            case P1:
               min = path1;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j;
               if (min >= path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               cell[i][j].global = min;
               break;
            case P2:
               cell[i][j].global = path1;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j;
               break;
            case P3:
               cell[i][j].global = path2;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
            default:
               break;
            }
         }
      }
   }
   cell[Tx - 1][Ty - 1].global /=(Tx + Ty);
}

void recursive_calc_type_4(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   double local = 0.0, min = 0.0, path1, path2, path3;
   DTW_Cell **cell = table->cell;
   WEIGHT weight = table->weight;

   for (i = 1; i < Tx; i++) {
      if (cell[i][0].is_region == PATH_OK) {
         cell[i][0].global =
             cell[i - 1][0].global +weight.val[0] * cell[i][0].local;
         cell[i][0].backptr[0] = i - 1;
         cell[i][0].backptr[1] = 0;
      }
   }

   cell[1][1].global = cell[0][0].global +weight.val[1] * cell[1][1].local;
   cell[1][1].backptr[0] = 0;
   cell[1][1].backptr[1] = 0;

   for (i = 1; i < Tx; i++) {
      if (cell[i][1].is_region == PATH_OK) {
         min = cell[i - 1][1].global +weight.val[0] * local;
         cell[i][1].backptr[0] = i - 1;
         cell[i][1].backptr[1] = 1;
         if (min >= cell[i - 1][0].global +weight.val[1] * local) {
            min = cell[i - 1][0].global +weight.val[1] * local;
            cell[i][1].backptr[0] = i - 1;
            cell[i][1].backptr[1] = 0;
         }
         cell[i][1].global = min;
      }
   }
   cell[1][2].global = cell[0][0].global +weight.val[2] * cell[1][2].local;
   cell[1][2].backptr[0] = 0;
   cell[1][2].backptr[1] = 0;

   for (j = 2; j < Ty; j++) {
      for (i = 2; i < Tx; i++) {
         local = cell[i][j].local;
         if (cell[i][j].is_region == PATH_OK) {
            path1 = cell[i - 1][j].global +weight.val[0] * local;
            path2 = cell[i - 1][j - 1].global +weight.val[1] * local;
            path3 = cell[i - 1][j - 2].global +weight.val[2] * local;

            switch (cell[i][j].allow_path) {
            case P1:
               min = path1;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j;
               if (min >= path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P2:
               min = path1;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j;
               if (min >= path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               cell[i][j].global = min;
               break;
            case P3:
               min = path2;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P4:
               cell[i][j].global = path2;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               break;
            case P5:
               cell[i][j].global = path3;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 2;
               break;
            default:
               break;
            }
         }
      }
   }
   cell[Tx - 1][Ty - 1].global /=(Tx + Ty);
}

void recursive_calc_type_5(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   double local = 0.0, min = 0.0, path1, path2, path3;
   DTW_Cell **cell = table->cell;
   WEIGHT weight = table->weight;

   cell[2][1].global = cell[0][0].global +weight.val[0] * cell[1][1].local +
       weight.val[1] * cell[2][1].local;
   cell[2][1].backptr[0] = 0;
   cell[2][1].backptr[1] = 0;

   cell[1][1].global = cell[0][0].global +weight.val[2] * cell[1][1].local;
   cell[1][1].backptr[0] = 0;
   cell[1][1].backptr[1] = 0;

   cell[1][2].global = cell[0][0].global +weight.val[3] * cell[1][1].local +
       weight.val[4] * cell[1][2].local;
   cell[1][2].backptr[0] = 0;
   cell[1][2].backptr[1] = 0;

   for (j = 2; j < Ty - 1; j++) {
      for (i = 2; i < Tx - 1; i++) {
         local = cell[i][j].local;
         if (cell[i][j].is_region == PATH_OK) {
            path1 = cell[i - 2][j - 1].global +
                weight.val[0] * cell[i - 1][j].local +
                weight.val[1] * cell[i][j].local;
            path2 = cell[i - 1][j - 1].global +
                weight.val[2] * cell[i][j].local;
            path3 = cell[i - 1][j - 2].global +
                weight.val[3] * cell[i][j - 1].local +
                weight.val[4] * cell[i][j].local;

            switch (cell[i][j].allow_path) {
            case P1:
               min = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               if (min > path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               if (min > path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P2:
               min = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               if (min > path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               cell[i][j].global = min;
               break;
            case P3:
               min = path2;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               if (min > path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P4:
               cell[i][j].global = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               break;
            case P5:
               cell[i][j].global = path2;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               break;
            case P6:
               cell[i][j].global = path3;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 2;
               break;
            default:
               break;
            }
         }
      }
   }
   cell[Tx - 1][Ty - 1].backptr[0] = Tx - 2;
   cell[Tx - 1][Ty - 1].backptr[1] = Ty - 2;
   cell[Tx - 1][Ty - 1].global = cell[Tx - 2][Ty - 2].global /(Tx + Ty);
}

void recursive_calc_type_6(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   double local = 0.0, min = 0.0, path1, path2, path3;
   DTW_Cell **cell = table->cell;
   WEIGHT weight = table->weight;

   cell[2][1].global = cell[0][0].global +weight.val[0] * cell[2][1].local;
   cell[2][1].backptr[0] = 0;
   cell[2][1].backptr[1] = 0;

   cell[1][1].global = cell[0][0].global +weight.val[1] * cell[1][1].local;
   cell[1][1].backptr[0] = 0;
   cell[1][1].backptr[1] = 0;

   cell[1][2].global = cell[0][0].global +weight.val[2] * cell[1][2].local;
   cell[1][2].backptr[0] = 0;
   cell[1][2].backptr[1] = 0;

   for (j = 2; j < Ty; j++) {
      for (i = 2; i < Tx; i++) {
         local = cell[i][j].local;
         if (cell[i][j].is_region == PATH_OK) {
            path1 = cell[i - 2][j - 1].global +weight.val[0] * local;
            path2 = cell[i - 1][j - 1].global +weight.val[1] * local;
            path3 = cell[i - 1][j - 2].global +weight.val[2] * local;

            switch (cell[i][j].allow_path) {
            case P1:
               min = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P2:
               min = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               cell[i][j].global = min;
               break;
            case P3:
               min = path2;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P4:
               cell[i][j].global = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               break;
            case P5:
               cell[i][j].global = path2;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               break;
            case P6:
               cell[i][j].global = path3;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 2;
               break;
            default:
               break;
            }
         }
      }
   }
   cell[Tx - 1][Ty - 1].global /=(Tx + Ty);
}

void recursive_calc_type_7(DTW_Table * table)
{
   int i, j, Tx = table->data[0].total, Ty = table->data[1].total;
   double local = 0.0, min = 0.0, path1, path2, path3, path4;
   DTW_Cell **cell = table->cell;
   WEIGHT weight = table->weight;

   cell[1][1].global = cell[0][0].global +weight.val[4] * cell[1][1].local;
   cell[1][1].backptr[0] = 0;
   cell[1][1].backptr[1] = 0;

   cell[1][2].global = cell[0][0].global +weight.val[5] * cell[1][2].local;
   cell[1][2].backptr[0] = 0;
   cell[1][2].backptr[1] = 0;

   cell[2][1].global = cell[0][0].global +weight.val[0] * cell[1][1].local +
       weight.val[1] * cell[2][1].local;
   cell[2][1].backptr[0] = 0;
   cell[2][1].backptr[1] = 0;

   for (j = 2; j < Ty - 1; j++) {
      for (i = 2; i < Tx - 1; i++) {
         local = cell[i][j].local;
         if (cell[i][j].is_region == PATH_OK) {
            path1 = cell[i - 2][j - 1].global +
                weight.val[0] * cell[i - 1][j].local + weight.val[1] * local;
            path2 = cell[i - 2][j - 2].global +
                weight.val[2] * cell[i - 1][j].local + weight.val[3] * local;
            path3 = cell[i - 1][j - 1].global +weight.val[4] * local;
            path4 = cell[i - 1][j - 2].global +weight.val[5] * local;

            switch (cell[i][j].allow_path) {
            case P1:
               min = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 2;
                  cell[i][j].backptr[1] = j - 2;
               }
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               if (min >= path4) {
                  min = path4;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P2:
               min = path2;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 2;
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               if (min >= path4) {
                  min = path4;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P3:
               min = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path2) {
                  min = path2;
                  cell[i][j].backptr[0] = i - 2;
                  cell[i][j].backptr[1] = j - 2;
               }
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               cell[i][j].global = min;
               break;
            case P4:
               min = path3;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path4) {
                  min = path4;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P5:
               min = path2;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 2;
               if (min >= path4) {
                  min = path4;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 2;
               }
               cell[i][j].global = min;
               break;
            case P6:
               min = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               if (min >= path3) {
                  min = path3;
                  cell[i][j].backptr[0] = i - 1;
                  cell[i][j].backptr[1] = j - 1;
               }
               cell[i][j].global = min;
               break;
            case P7:
               cell[i][j].global = path1;
               cell[i][j].backptr[0] = i - 2;
               cell[i][j].backptr[1] = j - 1;
               break;
            case P8:
               cell[i][j].global = path3;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 1;
               break;
            case P9:
               cell[i][j].global = path4;
               cell[i][j].backptr[0] = i - 1;
               cell[i][j].backptr[1] = j - 2;
               break;
            default:
               break;
            }
         }
      }
   }
   cell[Tx - 1][Ty - 1].backptr[0] = Tx - 2;
   cell[Tx - 1][Ty - 1].backptr[1] = Ty - 2;
   cell[Tx - 1][Ty - 1].global = cell[Tx - 2][Ty - 2].global /Tx;
}

/* Check and mark region where global cost can be calculated */
void check_enabled_region(DTW_Table * table)
{
   switch (table->path) {
   case I:                     /* horizontal and vertical */
      check_enabled_region_type_1(table);
      break;
   case II:                    /* horizontal, oblique and vertical */
      check_enabled_region_type_2(table);
      break;
   case III:                   /* horizontal and oblique */
      check_enabled_region_type_3(table);
      break;
   case IV:                    /* horizontal, oblique1, oblique2 */
      check_enabled_region_type_4(table);
      break;
   case V:                     /* default */
      check_enabled_region_type_5(table);
      break;
   case VI:
      check_enabled_region_type_6(table);
      break;
   case VII:
      check_enabled_region_type_7(table);
      break;
   default:
      break;
   }
}

/* Calculate local cost */
void calc_local_cost(DTW_Table * table)
{
   int i, j, d, D = table->data[0].dim;
   double sum;
   DTW_Cell **cell = table->cell;
   DATA *data = table->data;
   enum Norm norm = table->norm;

   switch (norm) {
   case L1:
      for (i = 0; i < data[0].total; i++) {
         for (j = 0; j < data[1].total; j++) {
            if (cell[i][j].is_region == PATH_OK) {
               for (d = 0, sum = 0.0; d < D; d++) {
                  sum += fabs(data[0].input[i * D + d] -
                              data[1].input[j * D + d]);
               }
               cell[i][j].local = sum;
            }
         }
      }
      break;
   case L2:
      for (i = 0; i < data[0].total; i++) {
         for (j = 0; j < data[1].total; j++) {
            if (cell[i][j].is_region == PATH_OK) {
               for (d = 0, sum = 0.0; d < D; d++) {
                  sum += pow((data[0].input[i * D + d] -
                              data[1].input[j * D + d]), 2);
               }
               cell[i][j].local = sqrt(sum);
            }
         }
      }
      break;
   default:
      break;
   }
}

/* Calculate global cost recursively */
void recursive_calc(DTW_Table * table)
{
   table->cell[0][0].global = table->cell[0][0].local;
   table->cell[0][0].backptr[0] = -1;
   table->cell[0][0].backptr[1] = -1;

   switch (table->path) {
   case I:
      recursive_calc_type_1(table);
      break;
   case II:
      recursive_calc_type_2(table);
      break;
   case III:
      recursive_calc_type_3(table);
      break;
   case IV:
      recursive_calc_type_4(table);
      break;
   case V:
      recursive_calc_type_5(table);
      break;
   case VI:
      recursive_calc_type_6(table);
      break;
   case VII:
      recursive_calc_type_7(table);
      break;
   default:
      break;
   }
}

/* Obtain Viterbi path */
void back_trace(DTW_Table * table)
{
   int k, l, Tx = table->data[0].total, Ty = table->data[1].total,
       *back_x, *back_y, *phi_x, *phi_y;
   DTW_Cell **cell = table->cell;
   DATA *data = table->data;
   enum PATH path = table->path;

   back_x = (int *) malloc(sizeof(int) * (Tx + Ty));
   back_y = (int *) malloc(sizeof(int) * (Tx + Ty));

   phi_x = (int *) malloc(sizeof(int) * (Tx + Ty));
   phi_y = (int *) malloc(sizeof(int) * (Tx + Ty));

   back_x[0] = phi_x[0] = Tx - 1;
   back_y[0] = phi_y[0] = Ty - 1;
   k = l = 1;

   while (back_x[l - 1] != 0 && back_y[l - 1] != 0) {
      back_x[l] = cell[back_x[l - 1]][back_y[l - 1]].backptr[0];
      back_y[l] = cell[back_x[l - 1]][back_y[l - 1]].backptr[1];
      switch (path) {
      case V:
         if (back_x[l - 1] - back_x[l] == 2 && back_y[l - 1] - back_y[l] == 1) {
            phi_x[k] = back_x[l - 1] - 1;
            phi_y[k] = back_y[l - 1];
            phi_x[k + 1] = back_x[l];
            phi_y[k + 1] = back_y[l];
            k += 2;
         } else if (back_x[l - 1] - back_x[l] == 1
                    && back_y[l - 1] - back_y[l] == 1) {
            phi_x[k] = back_x[l];
            phi_y[k] = back_y[l];
            k++;
         } else if (back_x[l - 1] - back_x[l] == 1
                    && back_y[l - 1] - back_y[l] == 2) {
            phi_x[k] = back_x[l - 1];
            phi_y[k] = back_y[l - 1] - 1;
            phi_x[k + 1] = back_x[l];
            phi_y[k + 1] = back_y[l];
            k += 2;
         }
         break;
      case VII:
         if (back_x[l - 1] - back_x[l] == 2 && back_y[l - 1] - back_y[l] == 1) {
            phi_x[k] = back_x[l - 1] - 1;
            phi_y[k] = back_y[l - 1];
            phi_x[k + 1] = back_x[l];
            phi_y[k + 1] = back_y[l];
            k += 2;
         } else if (back_x[l - 1] - back_x[l] == 2
                    && back_y[l - 1] - back_y[l] == 2) {
            phi_x[k] = back_x[l - 1] - 1;
            phi_y[k] = back_y[l - 1];
            phi_x[k + 1] = back_x[l];
            phi_y[k + 1] = back_y[l];
            k += 2;
         } else if (back_x[l - 1] - back_x[l] == 1
                    && back_y[l - 1] - back_y[l] == 1) {
            phi_x[k] = back_x[l];
            phi_y[k] = back_y[l];
            k++;
         } else if (back_x[l - 1] - back_x[l] == 1
                    && back_y[l - 1] - back_y[l] == 2) {
            phi_x[k] = back_x[l];
            phi_y[k] = back_y[l];
            k++;
         }
         break;
      default:
         phi_x[k] = back_x[l];
         phi_y[k] = back_y[l];
         k++;
         break;
      }
      l++;
   }

   table->vit_leng = k;
   for (k = 0; k < table->vit_leng; k++) {
      data[0].viterbi[k] = phi_x[table->vit_leng - k - 1];
      data[1].viterbi[k] = phi_y[table->vit_leng - k - 1];
   }

   free(back_x);
   free(back_y);
   free(phi_x);
   free(phi_y);
}

/* Concatenate two input vectors along Viterbi path */
double *concat(DTW_Table * table)
{
   double *concat;
   int i, j, size = table->vit_leng,
       dim = table->data[0].dim + table->data[1].dim;
   DATA *data = table->data;

   concat = dgetmem(size * dim);

   for (i = 0; i < size; i++) {
      for (j = 0; j < data[0].dim; j++) {
         concat[dim * i + j]
             = data[0].input[data[0].viterbi[i] * data[0].dim + j];
      }
      for (j = 0; j < data[1].dim; j++) {
         concat[dim * i + data[0].dim + j]
             = data[1].input[data[1].viterbi[i] * data[1].dim + j];
      }
   }

   return (concat);
}

/* Perform dynamic time warping */
void dtw(DTW_Table * table, double **output)
{
   /* Check and mark region where global cost can be calculated */
   check_enabled_region(table);

   /* Calculate local cost */
   calc_local_cost(table);

   /* Calculate global cost recursively */
   recursive_calc(table);

   /* Obtain Viterbi path */
   back_trace(table);

   /* Concatenate two input vectors along Viterbi path */
   *output = concat(table);
}

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - Dynamic Time Warping\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] reffile [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m M      : order of vector                      [%d]\n",
           LENG);
   fprintf(stderr,
           "       -l L      : dimension of vector                  [m+1]\n");
   fprintf(stderr,
           "       -t T      : number of test vectors               [N/A]\n");
   fprintf(stderr,
           "       -r R      : number of reference vectors          [N/A]\n");
   fprintf(stderr,
           "       -n N      : type of norm used for calculation    [%d]\n",
           L2);
   fprintf(stderr, "                   of local cost\n");
   fprintf(stderr, "                      N = 1 : L1-norm\n");
   fprintf(stderr, "                      N = 2 : L2-norm\n");
   fprintf(stderr,
           "       -p P      : local path constraint                [%d]\n", V);
   fprintf(stderr,
           "       -s sfile  : output score of dynamic time warping [FALSE]\n");
   fprintf(stderr, "                   to sfile \n");
   fprintf(stderr,
           "       -v vfile  : output frame number sequence         [FALSE]\n");
   fprintf(stderr, "                   along the Viterbi path\n");
   fprintf(stderr, "       -h        : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       test vector sequence (%s)                     [stdin]\n",
           FORMAT);
   fprintf(stderr, "  reffile:\n");
   fprintf(stderr,
           "       reference vector sequence (%s)             \n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       concatenated test and reference vectors\n");
   fprintf(stderr, "       along the Viterbi path (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

int main(int argc, char *argv[])
{
   char *infile2 = NULL, *Scorefile = NULL, *Viterbifile = NULL;
   int i, dim = LENG, num_test, num_ref, length_test = 0, length_ref = 0;
   double *x = NULL, *y = NULL, *z = NULL;
   enum Norm norm_type = L2;
   enum PATH path_type = V;
   FILE *fp = stdin, *fp2 = NULL, *fpScore = NULL, *fpViterbi = NULL;
   Boolean outscore = FA, outViterbi = FA;
   DTW_Table table;

   if ((cmnd = strrchr(argv[0], '/')) == NULL) {
      cmnd = argv[0];
   } else {
      cmnd++;
   }
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':
            dim = atoi(*++argv) + 1;
            --argc;
            break;
         case 'l':
            dim = atoi(*++argv);
            --argc;
            break;
         case 't':
            length_test = atoi(*++argv);
            --argc;
            break;
         case 'r':
            length_ref = atoi(*++argv);
            --argc;
            break;
         case 'n':
            norm_type = (enum PATH) atoi(*++argv);
            --argc;
            break;
         case 'p':
            path_type = (enum PATH) atoi(*++argv);
            --argc;
            break;
         case 's':
            Scorefile = *++argv;
            fpScore = getfp(Scorefile, "wb");
            outscore = TR;
            --argc;
            break;
         case 'v':
            Viterbifile = *++argv;
            fpViterbi = getfp(Viterbifile, "wb");
            outViterbi = TR;
            --argc;
            break;
         case 'h':
            usage(EXIT_SUCCESS);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(EXIT_FAILURE);
         }
      } else if (infile2 == NULL) {
         infile2 = *argv;
         fp2 = getfp(infile2, "rb");
      } else {
         fp = getfp(*argv, "rb");
      }
   }

   if (infile2 == NULL) {
      fprintf(stderr, "%s : The reference file name must be specified !\n",
              cmnd);
      usage(EXIT_FAILURE);
   }

   if (outscore == TR && fpScore == NULL) {
      fprintf(stderr, "%s : output file name must be specified !\n", cmnd);
      usage(EXIT_FAILURE);
   }
   if (outViterbi == TR && fpViterbi == NULL) {
      fprintf(stderr, "%s : output file name must be specified !\n", cmnd);
      usage(EXIT_FAILURE);
   }

   x = read_input(fp, dim, &num_test);  /* test vectors */
   y = read_input(fp2, dim, &num_ref);  /* reference vectors */

   if (length_test != 0) {      /* if -t option is specified */
      num_test = length_test;
   }
   if (length_ref != 0) {       /* if -r option is specified */
      num_ref = length_ref;
   }

   /* Initialize */
   init_dtw(&table, dim, x, y, num_test, num_ref, path_type, norm_type);

   /* Perform dynamic time warping */
   dtw(&table, &z);

   /* output */
   fwritef(z, sizeof(*z), table.vit_leng * 2 * dim, stdout);
   if (outscore == TR) {
      fwritef(&table.cell[num_test - 1][num_ref - 1].global,
              sizeof(double), 1, fpScore);
   }
   if (outViterbi == TR) {
      for (i = 0; i < table.vit_leng; i++) {
         fwrite(table.data[0].viterbi + i, sizeof(int), 1, fpViterbi);
         fwrite(table.data[1].viterbi + i, sizeof(int), 1, fpViterbi);
      }
   }

   return (EXIT_SUCCESS);
}
