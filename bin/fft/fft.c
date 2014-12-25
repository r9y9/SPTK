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

/************************************************************************
*                                                                       *
*    FFT for Complex Sequence                                           *
*                                                                       *
*                                       1995.12 N.Isshiki modified      *
*       usage:                                                          *
*               fft [ options ] [ infile ] > stdout                     *
*       options:                                                        *
*               -l l     :  FFT size power of 2         [256]           *
*               -m m     :  order of sequence           [l-1]           *
*               -A       :  amplitude                   [FALSE]         *
*               -R       :  real part                   [FALSE]         *
*               -I       :  imaginary part              [FALSE]         *
*               -P       :  power                       [FALSE]         *
*       infile:                                                         *
*               stdin for default                                       *
*               input is assumed to be double                           *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: fft.c,v 1.29 2014/12/11 08:30:34 uratec Exp $";

/* Standard C Libraries */
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

/* Default Values */
#define SIZE 256

/* Command Name */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - FFT for complex sequence\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : FFT size  power of 2 [%d]\n", SIZE);
   fprintf(stderr, "       -m m  : order of sequence    [l-1]\n");
   fprintf(stderr, "       -A    : amplitude            [FALSE]\n");
   fprintf(stderr, "       -R    : real part            [FALSE]\n");
   fprintf(stderr, "       -I    : imaginary part       [FALSE]\n");
   fprintf(stderr, "       -P    : power                [FALSE]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)        [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       FFT sequence (%s)\n", FORMAT);
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
   int size = SIZE, size2, k, nd = -1, out = ' ';
   double *x, *y;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 'l':
            size = atoi(*++argv);
            argc--;
            break;
         case 'm':
            nd = atoi(*++argv) + 1;
            argc--;
            break;
         case 'i':
         case 'p':
         case 'r':
            c -= ('a' - 'A');
         case 'A':
         case 'I':
         case 'P':
         case 'R':
            out = c;
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
      return (1);
   }

   fp = stdin;

   if (infile) {
      fp = getfp(infile, "rb");
   }

   x = dgetmem(size2 = size + size);
   y = x + size;

   while (!feof(fp)) {
      fillz(x, size2, sizeof(double));
      if (freadf(x, sizeof(*x), nd, fp) == 0)
         break;
      if (freadf(y, sizeof(*y), nd, fp) == 0)
         break;
      fft(x, y, size);
      if (out == 'P')
         for (k = 0; k < size; k++)
            x[k] = x[k] * x[k] + y[k] * y[k];
      else if (out == 'A')
         for (k = 0; k < size; k++)
            x[k] = sqrt(x[k] * x[k] + y[k] * y[k]);
      if (out != 'I')
         fwritef(x, sizeof(*x), size, stdout);
      if (out == ' ' || out == 'I')
         fwritef(y, sizeof(*y), size, stdout);
   }

   if (infile) {
      fclose(fp);
   }

   return (0);
}
