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
*    IFFT for Real Sequence                                             *
*                                                                       *
*                                       2012.8    Akira Tamamori        *
*                                                                       *
*       usage:                                                          *
*               ifftr [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -l l     :  FFT size power of 2         [256]           *
*               -m m     :  order of sequence           [l-1]           *
*       infile:                                                         *
*               stdin for default                                       *
*               input is assumed to be float                            *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: ifftr.c,v 1.2 2013/12/16 09:01:58 mataki Exp $";


/* Standard C Libraries */
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

/* Default Values */
#define SIZE 256

/* Command Name */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - IFFT for real sequence\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : FFT size power of 2 [%d]\n", SIZE);
   fprintf(stderr, "       -m m  : order of sequence   [l-1]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       FFT sequence (%s)       [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       data sequence (%s)\n", FORMAT);
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
   FILE *fp;
   char *s, *infile = NULL, c;
   int size = SIZE, nd = -1, size2;
   double *x, *y;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if ((c == 'l' || c == 'm') && (*++s == '\0')) {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'l':
            size = atoi(s);
            break;
         case 'm':
            nd = atoi(s) + 1;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, c);
            usage(1);
         }
      } else
         infile = s;
   }

   if (nd == -1)
      nd = size;
   if (nd > size) {
      fprintf(stderr,
              "%s : Order of sequence %d should be less than the FFT size %d!\n",
              cmnd, nd, size);
      usage(1);
   }

   fp = stdin;

   if (infile) {
      fp = getfp(infile, "rb");
   }

   x = dgetmem(size2 = size + size);
   y = x + size;

   while (!feof(fp)) {
      fillz(x, size2, sizeof(*x));
      if (freadf(x, sizeof(*x), nd, fp) == 0)
         break;
      if (freadf(y, sizeof(*y), nd, fp) == 0)
         break;

      ifft(x, y, nd);

      /* output real sequence */
      fwritef(x, sizeof(*x), size, stdout);
   }

   if (infile) {
      fclose(fp);
   }

   return (0);
}
