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
*    DCT : Discrete Cosine Transform                                    *
*                                                                       *
*                                                                       *
*       usage:                                                          *
*               dct [ options ] [ infile ] > stdout                     *
*       options:                                                        *
*               -l l     :  DCT size                    [256]           *
*               -I       :  use complex number          [FALSE]         *
*               -d       : without using fft algorithm  [FALSE]         *
*       infile:                                                         *
*               stdin for default                                       *
*               input is assumed to be double                           *
*                                                                       *
************************************************************************/
static char *rcs_id = "$Id: dct.c,v 1.22 2013/12/16 09:01:55 mataki Exp $";


/*  Standard C Libraries  */
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

static int size = 256, out = ' ';

/* Default Values */
#define SIZE 256
#define DFTMODE FA
#define COMPMODE FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;

#include <memory.h>

int usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s -dct: Discrete Cosine Transform\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : DCT size             [%d]\n", SIZE);
   fprintf(stderr, "       -I    : use comlex number       [FALSE]\n");
   fprintf(stderr,
           "       -d    : without using fft algorithm (use dft) [%s]\n",
           BOOL[DFTMODE]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)        [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       DCT sequence (%s)\n", FORMAT);
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
   FILE *fp;
   char *s, *infile = NULL, c;

   FILE *getfp();
   Boolean dftmode = DFTMODE;
   Boolean compmode = COMPMODE;
   double *x, *y, *pReal2, *pImag2, *dgetmem();
   int size2;


   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if ((c == 'l') && (*++s == '\0')) {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'l':
            size = atoi(s);
            break;
         case 'I':
            out = c;
            compmode = 1 - compmode;
            break;
         case 'd':
            dftmode = 1 - dftmode;
            break;
         case 'h':
         default:
            usage();
         }
      } else
         infile = s;
   }
   if (infile)
      fp = getfp(infile, "rb");
   else
      fp = stdin;

   x = dgetmem(size2 = size + size);
   pReal2 = dgetmem(size2 = size + size);
   y = x + size;
   pImag2 = pReal2 + size;

   while (!feof(fp)) {
      fillz(x, size2, sizeof(double));
      fillz(y, size, sizeof(double));
      if (freadf(x, sizeof(*x), size, fp) == 0)
         break;
      if (out == 'I') {
         if (freadf(y, sizeof(*y), size, fp) == 0)
            break;
      }

      dct(x, pReal2, size, size, dftmode, compmode);

      fwritef(pReal2, sizeof(*pReal2), size, stdout);
      if (out == 'I')
         fwritef(pImag2, sizeof(*pReal2), size, stdout);
   }

   if (infile)
      fclose(fp);

   return (0);
}
