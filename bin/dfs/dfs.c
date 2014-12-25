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

/**************************************************************************
*                                                                         *
*    Digital Filter in Standard Form                                      *
*                                                                         *
*                                       1989.6   K.Tokuda                 *
*                                       1995.12  N.Isshiiki modified      *
*       usage:                                                            *
*               dfs [ options ] [ infile ] > stdout                       *
*       options:                                                          *
*               -a k a1 a2 ... aM  : denominator coefficients      [N/A]  *
*               -b b0 b1 b2 ... bM : numerator coefficients        [N/A]  *
*               -p pfile           : denominator coefficients file [NULL] *
*               -z zfile           : numerator coefficients file   [NULL] *
*       infile:                                                           *
*               input (float)                                             *
*       stdout:                                                           *
*               output (float)                                            *
*                                                                         *
**************************************************************************/

static char *rcs_id = "$Id: dfs.c,v 1.28 2014/12/11 08:30:33 uratec Exp $";


/* Standard C Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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

/* Command Name */
char *cmnd;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - digital filter in standard form\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout \n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -a K  a1...aM : denominator coefficients      [N/A]\n");
   fprintf(stderr,
           "       -b b0 b1...bN : numerator coefficients        [N/A]\n");
   fprintf(stderr,
           "       -p pfile      : denominator coefficients file [NULL]\n");
   fprintf(stderr,
           "       -z zfile      : numerator coefficients file   [NULL]\n");
   fprintf(stderr, "       -h            : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       filter input (%s)                          [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       filter output (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

double *read_coeff(int *argc, char ***argv, int *num)
{
   int i;
   float_list *top, *cur, *prev, *tmpf, *tmpff;
   double *coeff = NULL;

   top = prev = (float_list *) malloc(sizeof(float_list));
   top->f = fgetmem(1);
   while ((*argc - 1) && !isalpha(*(*(*argv + 1) + 1))) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = fgetmem(1);
      cur->f[0] = atof(*++(*argv));
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
      (*argc)--;
      (*num)++;
   }
   coeff = dgetmem(*num + 1);
   for (i = 0, tmpf = top->next; tmpf != NULL; i++, tmpf = tmpff) {
      coeff[i] = tmpf->f[0];
      tmpff = tmpf->next;
      free(tmpf->f);
      free(tmpf);
   }
   free(top);

   return (coeff);
}

double *read_coeff_file(FILE * fp, int *num)
{
   int i;
   float_list *top, *cur, *prev, *tmpf, *tmpff;
   double *dat, *coeff = NULL;

   top = prev = (float_list *) malloc(sizeof(float_list));
   top->f = fgetmem(1);
   dat = dgetmem(1);
   while (freadf(dat, sizeof(*dat), 1, fp) == 1) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = fgetmem(1);
      cur->f[0] = dat[0];
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
      (*num)++;
   }
   coeff = dgetmem(*num + 1);
   for (i = 0, tmpf = top->next; tmpf != NULL; i++, tmpf = tmpff) {
      coeff[i] = tmpf->f[0];
      tmpff = tmpf->next;
      free(tmpf->f);
      free(tmpf);
   }
   free(top);
   free(dat);

   return (coeff);
}

int main(int argc, char *argv[])
{
   double x, *a = NULL, *b = NULL, *d = NULL;
   int bufp = 0, max = 0, na = -1, nb = -1;
   char *file_z = "", *file_p = "";
   FILE *fp_z = NULL, *fp_p = NULL;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-')
         switch (*(*argv + 1)) {
         case 'a':
            a = read_coeff(&argc, &argv, &na);
            break;
         case 'b':
            b = read_coeff(&argc, &argv, &nb);
            break;
         case 'z':
            argc--;
            file_z = *++argv;
            break;
         case 'p':
            argc--;
            file_p = *++argv;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
      } else {
         fprintf(stderr, "%s : Invalid option!\n", cmnd);
         usage(1);
      }

   if (*file_z != '\0') {
      fp_z = getfp(file_z, "rb");
      b = read_coeff_file(fp_z, &nb);
   }
   if (*file_p != '\0') {
      fp_p = getfp(file_p, "rb");
      a = read_coeff_file(fp_p, &na);
   }

   if (na == -1) {
      a = dgetmem(1);
      na = 0;
      a[0] = 1.0;
   }
   if (nb == -1) {
      b = dgetmem(1);
      nb = 0;
      b[0] = 1.0;
   }

   max = (nb > na) ? (nb + 1) : (na + 1);
   d = dgetmem(max);
   while (freadf(&x, sizeof(x), 1, stdin) == 1) {
      x = dfs(x, a, na, b, nb, d, &bufp);
      fwritef(&x, sizeof(x), 1, stdout);
   }

   return 0;
}
