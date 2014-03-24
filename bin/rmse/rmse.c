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

/************************************************************************
*                                                                       *
*    Calculation of Root Mean Squared Error                             *
*                                                                       *
*                                     1996.3  K.Koishida                *
*                                     2013.7  T.Aritake                 *
*                                                                       *
*       usage:                                                          *
*               rmse [ options ] file1 [ infile ] > stdout              *
*       options:                                                        *
*               -l l         :  length of vector    [0]                 *
*               -n n         :  order of vector     [l-1]               *
*               -t t         :  number of vector    [EOD]               *
*               -magic magic :  remove magic number                     *
*               -MAGIC MAGIC :  replace magic number by MAGIC           *
*                               if -magic option is not given,          *
*                               return error                            *
*                               if -magic or -MAGIC option              *
*                               is given multiple times, return error   *
*       infile:                                                         *
*               data sequence                                           *
*                       , x(0), x(1), ..., x(l-1),                      *
*       stdout:                                                         *
*               rmse                                                    *
*                       , x(0)+x(1)...+x(l-1)/l                         *
*       notice:                                                         *
*               if l>0, calculate rmse frame by frame                   *
*       require:                                                        *
*               rmse()                                                  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: rmse.c,v 1.27 2013/12/18 03:58:38 mataki Exp $";


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
#define LENG 0
#define MAGIC 0
#define REP 0

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - calculation of root mean squared error\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] file1 [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l         : length of vector    [%d]\n", LENG);
   fprintf(stderr, "       -n n         : order of vector     [l-1]\n");
   fprintf(stderr, "       -t t         : number of vector    [EOD]\n");
   fprintf(stderr, "       -magic magic : remove magic number\n");
   fprintf(stderr, "       -MAGIC MAGIC : replace magic number by MAGIC\n");
   fprintf(stderr, "                      if -magic option is not given\n");
   fprintf(stderr, "                      return error\n");
   fprintf(stderr, "                      if -magic or -MAGIC option\n");
   fprintf(stderr,
           "                      is given multiple times, return error\n");
   fprintf(stderr, "       -h           : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "      data sequence (%s)               [stdin]\n", FORMAT);
   fprintf(stderr, "  file1:\n");
   fprintf(stderr, "      data sequence (%s)\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "      root mean squared error (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "      if l>0, calculate rmse frame by frame\n");
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
   int l = LENG, num = 0, i = 0, break_flag = 0, tv = -1;
   FILE *fp = stdin, *fp1 = NULL;
   double *x, *y, x1, y1, sub, z = 0.0;
   double magic = 0.0, rep = 0.0;
   int magic_flag = 0, magic_count = MAGIC, rep_count = REP;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            l = atoi(*++argv);
            --argc;
            break;
         case 'n':
            l = atoi(*++argv) + 1;
            --argc;
            break;
         case 't':
            tv = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         case 'm':
            if (magic_count > 0) {
               fprintf(stderr,
                       "%s : Cannot specify -magic option multiple times!\n",
                       cmnd);
               usage(1);
            }
            if ((*(argv + 1)) == NULL) {        /* No magic number */
               fprintf(stderr,
                       "%s : -magic option need magic number !\n", cmnd);
               usage(1);
            }

            if (isdigit(**(argv + 1)) == 0) {   /* Check the magic number is correct */
               if (**(argv + 1) != '+' && **(argv + 1) != '-') {
                  fprintf(stderr,
                          "%s : -magic option need numerical number !\n", cmnd);
                  usage(1);
               } else if (isdigit(*(*(argv + 1) + 1)) == 0) {
                  fprintf(stderr,
                          "%s : -magic option need numerical number !\n", cmnd);
                  usage(1);
               }
            }

            magic = atof(*++argv);
            magic_flag = 1;
            magic_count++;
            --argc;
            break;
         case 'M':
            if (rep_count > 0) {
               fprintf(stderr,
                       "%s : Cannot specify -MAGIC option multiple times!\n",
                       cmnd);
               usage(1);
            }
            if (magic_flag == 0) {
               fprintf(stderr,
                       "%s : Cannot find -magic option before -MAGIC option!\n",
                       cmnd);
               usage(1);
            }
            if ((*(argv + 1)) == NULL) {        /* No magic number */
               fprintf(stderr,
                       "%s : -MAGIC option need magic number !\n", cmnd);
               usage(1);
            }

            if (isdigit(**(argv + 1)) == 0) {   /* Check the MAGIC number is correct */
               if (**(argv + 1) != '+' && **(argv + 1) != '-') {
                  fprintf(stderr,
                          "%s : -MAGIC option need numerical number !\n", cmnd);
                  usage(1);
               } else if (isdigit(*(*(argv + 1) + 1)) == 0) {
                  fprintf(stderr,
                          "%s : -MAGIC option need numerical number !\n", cmnd);
                  usage(1);
               }
            }

            rep = atof(*++argv);
            rep_count++;
            --argc;
            break;
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (fp1 == NULL)
         fp1 = getfp(*argv, "rb");
      else
         fp = getfp(*argv, "rb");

   if (l > 0) {
      x = dgetmem(l + l);
      y = x + l;
      while (freadf(x, sizeof(*x), l, fp) == l &&
             freadf(y, sizeof(*y), l, fp1) == l) {
         break_flag = 0;
         if (magic_count)
            for (i = 0; i < l; i++) {
               if (rep_count == 0) {
                  if (x[i] == magic || y[i] == magic)
                     break_flag = 1;
               } else {
                  if (x[i] == magic)
                     x[i] = rep;
                  if (y[i] == magic)
                     y[i] = rep;
               }
            }
         if (break_flag == 1)
            continue;

         z = rmse(x, y, l);
         fwritef(&z, sizeof(z), 1, stdout);

         tv--;
         if (tv == 0)
            break;
      }
   } else {
      while (freadf(&x1, sizeof(x1), 1, fp) == 1 &&
             freadf(&y1, sizeof(y1), 1, fp1) == 1) {
         if (magic_count)
            if (x1 == magic || y1 == magic) {
               if (rep_count) {
                  if (x1 == magic)
                     x1 = rep;
                  if (y1 == magic)
                     y1 = rep;
               } else {
                  return (0);
               }
            }
         sub = x1 - y1;
         z += sub * sub;
         num++;
      }
      z = sqrt(z /= num);
      fwritef(&z, sizeof(z), 1, stdout);

   }

   return (0);
}
