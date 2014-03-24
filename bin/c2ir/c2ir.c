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
*    Cepstrum to minimum phase Impulse Response                         *
*                                       1988.8  T.Kobayashi             *
*                                       1995.12 N.Isshiki   modified    *
*                                                                       *
*       usage:                                                          *
*               c2ir [ options ] [ infile ] > stdout                    *
*       options:                                                        *
*               -m m    :  order of cepstrum            [25]            *
*               -M M    :  order of impulse response    [255]           *
*               -l l    :  length of impulse responce   [256]           *
*               -i      :  input minimum phase sequence [FALSE]         *
*       infile:                                                         *
*               stdin for default                                       *
*               input is assumed to be float                            *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: c2ir.c,v 1.27 2013/12/16 09:01:54 mataki Exp $";


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


#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/* Default Values */
#define LENG 256
#define ORDER 25

/* Command Name */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - cepstrum to minimum phase impulse response\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of cepstrum            [%d]\n", ORDER);
   fprintf(stderr, "       -M M  : order of impulse response    [%d]\n",
           LENG - 1);
   fprintf(stderr, "       -l l  : length of impulse response   [%d]\n", LENG);
   fprintf(stderr, "       -i    : input minimum phase sequence [FALSE]\n");
   fprintf(stderr, "       -h    : print this message\n\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       cepstrum (%s)                     [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       impulse response (%s)\n", FORMAT);
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
   double *buf, *x;
   char *s, *infile = NULL, c;
   int nr;
   int leng = LENG, nc = ORDER + 1, is_i = 0;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if (c != 'i' && *++s == '\0') {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'i':
            is_i = 1;
            break;
         case 'm':
            nc = atoi(s) + 1;
            break;
         case 'M':
            leng = atoi(s) + 1;
            break;
         case 'l':
            leng = atoi(s);
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

   if (infile) {
      fp = getfp(infile, "rb");
   } else
      fp = stdin;

   nr = leng + nc;
   buf = dgetmem(nr);
   nr = (is_i) ? leng : nc;

   while (freadf(buf, sizeof(*buf), nr, fp) == nr) {
      if (!is_i) {
         x = buf + nc;
         c2ir(buf, nc, x, leng);
         fwritef(x, sizeof(*x), leng, stdout);
      } else {
         x = buf + leng;
         ic2ir(buf, leng, x, nc);
         fwritef(x, sizeof(*x), nc, stdout);
      }
   }

   return (0);
}
