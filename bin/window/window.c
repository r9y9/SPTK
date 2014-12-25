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
*    Data Windowing                                                     *
*                                                                       *
*                                      1996.1  N.Miyazaki               *
*                                      1998.11 T.Masuko                 *
*                                                                       *
*       usage:                                                          *
*               window [ options ] [ infile ] > outfile                 *
*       options:                                                        *
*               -l l  :  frame length of input    [256]                 *
*               -L L  :  frane length of output   [l]                   *
*               -n n  :  type of normalization    [1]                   *
*                          0 none                                       *
*                          1 normalize by power                         *
*                          2 normalize by magnitude                     *
*               -w w  :  type of window           [0]                   *
*                          0 (Blackman)                                 *
*                          1 (Hamming)                                  *
*                          2 (Hanning)                                  *
*                          3 (Bartlett)                                 *
*                          4 (trapezoid)                                *
*                          5 (rectangul)                                *
*       infile:                                                         *
*               stdin for default                                       *
*               input is assumed to be double                           *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: window.c,v 1.25 2014/12/11 08:30:52 uratec Exp $";


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

/*  Default Values */
#define FLENG  256
#define WINTYPE  BLACKMAN
#define NORMFLG  1

/*  Command Name  */
char *cmnd;


int usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - data windowing\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > outfile\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : frame length of input  [%d]\n", FLENG);
   fprintf(stderr, "       -L L  : frame length of output [l]\n");
   fprintf(stderr, "       -n n  : type of normalization  [%d]\n", NORMFLG);
   fprintf(stderr, "                 0 none\n");
   fprintf(stderr, "                 1 normalize by power\n");
   fprintf(stderr, "                 2 normalize by magnitude\n");
   fprintf(stderr, "       -w w  : type of window         [%d]\n", WINTYPE);
   fprintf(stderr, "                 0 (Blackman)\n");
   fprintf(stderr, "                 1 (Hamming)\n");
   fprintf(stderr, "                 2 (Hanning)\n");
   fprintf(stderr, "                 3 (Bartlett)\n");
   fprintf(stderr, "                 4 (trapezoid)\n");
   fprintf(stderr, "                 5 (rectangular)\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)          [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       windowed data sequence (%s)\n", FORMAT);
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
   FILE *fp = stdin;
   char *s, c;
   int fleng = FLENG, outl = -1, normflg = NORMFLG;
   Window wintype = WINTYPE;
   double *x;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if (*++s == '\0' && (c == 'n' || c == 'l' || c == 'w' || c == 'L')) {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'w':
            wintype = (Window) atoi(s);
            break;
         case 'l':
            fleng = atoi(s);
            break;
         case 'L':
            outl = atoi(s);
            break;
         case 'n':
            normflg = atoi(s);
            break;
         case 'h':
         default:
            usage();
         }
      } else
         fp = getfp(*argv, "rb");
   }

   if (outl < 0)
      outl = fleng;

   x = dgetmem(fleng > outl ? fleng : outl);

   while (freadf(x, sizeof(*x), fleng, fp) == fleng) {
      window(wintype, x, fleng, normflg);
      fwritef(x, sizeof(*x), outl, stdout);
   }

   return (0);
}
