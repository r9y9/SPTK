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
*    Transpose a matrix                                                 *
*                                                                       *
*                                        2012.8 T.Okada                 *
*                                                                       *
*       usage:                                                          *
*               transpose [ options ] [ infile ] > stdout               *
*       options:                                                        *
*               -m m  : number of rows      [N/A]                       *
*               -n n  : number of columns   [N/A]                       *
*       infile:                                                         *
*                x(0,0)  , x(0,1)  , ..., x(0,n-1)  ,                   *
*                x(1,0)  , x(1,1)  , ..., x(1,n-1)  ,                   *
*                              .                                        *
*                              .                                        *
*                              .                                        *
*                x(m-1,0), x(m-1,1), ..., x(m-1,n-1)                    *
*       stdout:                                                         *
*                x(0,0)  , x(1,0)  , ..., x(m-1,0)  ,                   *
*                x(0,1)  , x(1,1)  , ..., x(m-1,1)  ,                   *
*                              .                                        *
*                              .                                        *
*                              .                                        *
*                x(0,n-1), x(1,n-1), ..., x(m-1,n-1)                    *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: transpose.c,v 1.6 2013/12/16 09:02:04 mataki Exp $";


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


#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*   Command Name  */
char *cmnd;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

void usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transpose a matrix\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : number of rows       [N/A]\n");
   fprintf(stderr, "       -n n  : number of columns    [N/A]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)      [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       transposed data sequence (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(1);
}

int main(int argc, char *argv[])
{
   int i = 0, m = 0, n = 0, elnum = 0, count = 0;
   char *s, c;
   double *buf, *dat;
   FILE *fp = stdin;
   void transpose(double *, int, int);
   float_list *top = NULL, *prev = NULL, *cur = NULL;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if (*++s == '\0') {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'm':
            m = atoi(s);
            break;
         case 'n':
            n = atoi(s);
            break;
         case 'h':
            usage();
         default:
            usage();
         }
      } else
         fp = getfp(*argv, "rb");
   }

   if (m == 0 || n == 0) {
      fprintf(stderr, "%s : the number of rows or columns is not defined!!\n",
              cmnd);
      usage();
   }

   elnum = m * n;

   top = prev = (float_list *) getmem(1, sizeof(float_list));
   prev->next = NULL;
   dat = dgetmem(1);
   count = 0;
   while (freadf(dat, sizeof(*dat), 1, fp) == 1) {
      cur = (float_list *) getmem(1, sizeof(float_list));
      cur->f = fgetmem(1);
      cur->f[0] = (float) dat[0];
      count++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }

   if (count < elnum) {
      fprintf(stderr, "%s : input data is smaller than defined!!\n", cmnd);
      usage();
   }

   buf = dgetmem(count);
   for (i = 0, cur = top->next; i < count; i++, cur = cur->next) {
      buf[i] = (double) cur->f[0];
   }

   transpose(buf, m, n);

   return (0);
}
