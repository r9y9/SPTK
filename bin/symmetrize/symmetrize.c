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

/*************************************************************************************
*                                                                                    *
*    Symmetrize the sequence of data                                                 *
*                                                                                    *
*                                        2012.8 T.Okada                              *
*                                                                                    *
*     usage:                                                                         *
*             symmetrize [ options ] [ infile ] > stdout                             *
*     options:                                                                       *
*             -l L  : frame length   [256]                                           *
*             -o o  : output format    [0]                                           *
*     infile:                                                                        *
*             x(0), x(1),     ..., x(L/2-1)                                          *
*     stdout:                                                                        *
*             x(0), x(1),     ..., x(L/2-1), x(L/2-2), ..., x(2), x(1)     (if o==0) *
*             x(L/2-1), x(L/2-2), ..., x(1), x(0), x(1), ..., x(L/2-1)     (if o==1) *
*             x(L/2-1)/2, x(L/2-2), ..., x(1), x(0), x(1), ..., x(L/2-1)/2 (if o==2) *
*     notice:                                                                        *
*             value of L must be even number                                         *
*             value of L must be L>=4                                                *
*             value of L must be L>=6 (if o==0)                                      *
*                                                                                    *
**************************************************************************************/

static char *rcs_id =
    "$Id: symmetrize.c,v 1.9 2014/12/11 08:30:50 uratec Exp $";


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
#define OTYPE 0
#define FLENG 256

/*   Command Name  */
char *cmnd;


void usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - symmetrize the sequence of data\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l L  : frame length             [%d]\n", FLENG);
   fprintf(stderr, "       -o o  : output format            [%d]\n", OTYPE);
   fprintf(stderr,
           "               0 x(0), x(1),     ..., x(L/2-1), x(L/2-2), ..., x(2), x(1)\n");
   fprintf(stderr,
           "               1 x(L/2-1), x(L/2-2), ..., x(1), x(0), x(1), ..., x(L/2-1)\n");
   fprintf(stderr,
           "               2 x(L/2-1)/2, x(L/2-2), ..., x(1), x(0), x(1), ..., x(L/2-1)/2\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)      [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       symmetrized data sequence (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       value of L must be even number\n");
   fprintf(stderr, "       value of L must be L>=4\n");
   fprintf(stderr, "       value of L must be L>=6 (if o==0)\n");
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
   int i, o = OTYPE, L = FLENG;
   char *s, c;
   FILE *fp = stdin;
   double *buf, tmp;

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
            L = atoi(s);
            break;
         case 'o':
            o = atoi(s);
            break;
         case 'h':
            usage();
         default:
            usage();
         }
      } else
         fp = getfp(*argv, "rb");
   }

   if (L % 2 != 0) {
      fprintf(stderr, "%s : value of L must be even number!\n", cmnd);
      usage();
   }

   L /= 2;
   buf = dgetmem(L);

   if ((o != 0) && (o != 1) && (o != 2)) {
      fprintf(stderr, "%s : invalid output type %d\n", cmnd, o);
      usage();
   }
   if (L <= 1) {
      fprintf(stderr, "%s : value of L must be L>=4!\n", cmnd);
      usage();
   }

   while (freadf(buf, sizeof(*buf), L, fp) == L) {

      if (o == 0) {
         if (L <= 2) {
            fprintf(stderr, "%s : value of L must be L>=6! (if o==0)\n", cmnd);
            usage();
         }
         for (i = 0; i < L; i++) {
            fwritef(&buf[i], sizeof(*buf), 1, stdout);
         }
         for (i = L - 2; i > 0; i--) {
            fwritef(&buf[i], sizeof(*buf), 1, stdout);
         }
      } else if (o == 1) {
         for (i = L - 1; i > 0; i--) {
            fwritef(&buf[i], sizeof(*buf), 1, stdout);
         }
         for (i = 0; i < L; i++) {
            fwritef(&buf[i], sizeof(*buf), 1, stdout);
         }
      } else if (o == 2) {
         tmp = buf[L - 1] / 2;
         fwritef(&tmp, sizeof(*buf), 1, stdout);
         for (i = L - 2; i > 0; i--) {
            fwritef(&buf[i], sizeof(*buf), 1, stdout);
         }
         for (i = 0; i < L - 1; i++) {
            fwritef(&buf[i], sizeof(*buf), 1, stdout);
         }
         fwritef(&tmp, sizeof(tmp), 1, stdout);
      }
   }

   free(buf);
   fclose(fp);
   return (0);
}
