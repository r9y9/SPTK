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
*    Reverse the order of Data in Each Block                            *
*                                                                       *
*                                        1993.7 K.Tokuda                *
*                                        1996.1 N.miyazaki modified     *
*                                                                       *
*       usage:                                                          *
*               reverse [ options ] [ infile ] > stdout                 *
*       options:                                                        *
*               -l l  : length of block  [EOF]                          *
*               -n n  : order of block   [EOF-1]                        *
*       infile:                                                         *
*                       x0, x1, ..., xN, (real)                         *
*       stdout:                                                         *
*                       xN, ..., x1, x0, (real)                         *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: reverse.c,v 1.26 2013/12/16 09:02:03 mataki Exp $";


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

/*  Default Values  */
#define BLOCK 1

/*   Command Name  */
char *cmnd;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

void usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - reverse the order of data in each block\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : length of block       [EOF]\n");
   fprintf(stderr, "       -n n  : order of block        [EOF-1]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)      [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       reversed data sequence (%s)\n", FORMAT);
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
   char *s, c;
   int l = -1, length, block_size, i, j;
   double *x;
   FILE *fp = stdin;
   float_list *top, *cur, *prev, *tmpf;

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
         case 'l':
            l = atoi(s);
            break;
         case 'n':
            l = atoi(s) + 1;
            break;
         case 'h':
            usage();
         default:
            usage();
         }
      } else
         fp = getfp(*argv, "rb");
   }

   block_size = (l < 0) ? BLOCK : l;

   x = dgetmem(block_size);
   top = prev = (float_list *) malloc(sizeof(float_list));
   length = 0;
   prev->next = NULL;
   while (freadf(x, sizeof(*x), block_size, fp) == block_size) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = fgetmem(block_size);
      for (i = 0; i < block_size; i++) {
         cur->f[i] = (float) x[i];
      }
      length++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }
   free(x);

   if (l < 0) {
      x = dgetmem(length * block_size);
      for (i = 0, tmpf = top->next; tmpf != NULL; i++, tmpf = tmpf->next) {
         for (j = 0; j < block_size; j++) {
            x[i * block_size + j] = tmpf->f[j];
         }
      }
      reverse(x, length * block_size);
      fwritef(x, sizeof(*x), length * block_size, stdout);
   } else {
      x = dgetmem(block_size);
      for (tmpf = top->next; tmpf != NULL; tmpf = tmpf->next) {
         for (j = 0; j < block_size; j++) {
            x[j] = tmpf->f[j];
         }
         reverse(x, block_size);
         fwritef(x, sizeof(*x), block_size, stdout);
      }
   }

   return (0);
}
