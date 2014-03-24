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
*    2D Inverse FFT for complex sequence                                *
*                                                                       *
*                                       1989    T.Kobayashi             *
*                                       1996.2  N.Miyazaki modified     *
*                                                                       *
*       usage:                                                          *
*               ifft2 [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -l l    :  FFT size power of 2      [64]                *
*               +r      :  input is real            [FALSE]             *
*               -t      :  transposed output        [FALSE]             *
*               -c      :  compensated boundary     [FALSE]             *
*               -q      :  first quadrant output    [FALSE]             *
*               -R      :  output real part         [FALSE]             *
*               -I      :  output imaginary part    [FALSE]             *
*       infile:                                                         *
*               stdin for default                                       *
*               input is assumed to be complex                          *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: ifft2.c,v 1.26 2013/12/16 09:01:58 mataki Exp $";

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

static int size = 64, outopt = 0, out = ' ', is_fftr = 0;

/*  Command Name  */
static char *cmnd;


int usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - 2D inverse FFT for complex sequence\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : FFT size power of 2   [64]\n");
   fprintf(stderr, "       +r    : input is real         [FALSE]\n");
   fprintf(stderr, "       -t    : transposed output     [FALSE]\n");
   fprintf(stderr, "       -c    : compensated boundary  [FALSE]\n");
   fprintf(stderr, "       -q    : first quadrant output [FALSE]\n");
   fprintf(stderr, "       -R    : output real part      [FALSE]\n");
   fprintf(stderr, "       -I    : output imaginary part [FALSE]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)         [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       IFFT sequence (%s)\n", FORMAT);
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
   int size2, nread;
   double *x, *y;
   void trans(double *p);

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
         case 't':
         case 'c':
         case 'q':
            if ((c == 't') || (*++s == 't'))
               outopt = 1;
            if ((c == 'c') || (*s == 'c'))
               outopt = 2;
            if (c == 'q')
               outopt = -1;
            break;
         case 'i':
         case 'r':
            c -= ('a' - 'A');
         case 'I':
         case 'R':
            out = c;
            break;
         case 'h':
         default:
            usage();
         }
      } else if ((*s == '+') && (*(s + 1) == 'r'))
         is_fftr = 1 - is_fftr;
      else
         infile = s;
   }

   fp = stdin;

   if (infile) {
      fp = getfp(infile, "rb");
   }


   size2 = size * size;

   x = dgetmem(2 * size2);
   y = x + size2;
   nread = (is_fftr) ? size2 : 2 * size2;

   while (!feof(fp)) {
      if (freadf(x, sizeof(*x), nread, fp) != nread)
         break;
      if (is_fftr)
         fillz(y, sizeof(*y), size2 * (sizeof(double) / sizeof(long)));

      ifft2(x, y, size);

      if (out != 'I') {
         if (outopt)
            trans(x);
         else
            fwritef(x, sizeof(*x), size2, stdout);
      }
      if (out != 'R') {
         if (outopt)
            trans(y);
         else
            fwritef(y, sizeof(*y), size2, stdout);
      }
   }

   free(x);


   if (infile) {
      fclose(fp);
   }

   return (0);
}

void trans(double *p)
{
   int k, sizeh, nout;
   double *q;

   sizeh = size / 2;
   nout = (outopt == 1) ? sizeh : sizeh + 1;

   if (outopt > 0)
      for (q = p + sizeh * size, k = sizeh; --k >= 0; q += size) {
         fwritef(q + sizeh, sizeof(*p), sizeh, stdout);
         fwritef(q, sizeof(*p), nout, stdout);
      }

   for (q = p, k = nout; --k >= 0; q += size) {
      if (outopt > 0)
         fwritef(q + sizeh, sizeof(*p), sizeh, stdout);
      fwritef(q, sizeof(*p), nout, stdout);
   }
}
