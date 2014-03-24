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
*     Decoder of Multi Stage Vector Quantization                        *
*                                                                       *
*                                     1996.1  K.Koishida                *
*                                                                       * 
*        usage:                                                         *
*                imsvq [options] [ infile ] > stdout                    *
*        options:                                                       *
*                -l l   :  length of vector        [26]                 *
*                -n n   :  order of vector         [25]                 *
*                -s s f :  codebook                [N/A N/A]            *
*                     s :  codebook size                                *
*                     f :  codebook file                                *
*        infile:                                                        *
*                codebook index (int)                                   *
*                        index(0), index(1), ..., index(N),             *
*        stdout:                                                        *
*                quantized vector                                       *
*                        x'(0), x'(1), ..., x'(l-1),                    *
*        notice:                                                        *
*                -s option are specified number of stages               *
*        require:                                                       *
*                imsvq(), ivq()                                         *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: imsvq.c,v 1.21 2013/12/16 09:01:58 mataki Exp $";


/*  Standard C Libraries  */
#include <stdio.h>
#include <ctype.h>
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
#define LENG  26

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - decoder of multi stage vector quantization \n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "         -l l   : length of vector   [%d]\n", LENG);
   fprintf(stderr, "         -n n   : order of vector    [%d]\n", LENG - 1);
   fprintf(stderr, "         -s s f : codebook           [N/A N/A]\n");
   fprintf(stderr, "                   s: codebook size\n");
   fprintf(stderr, "                   f: codebook file\n");
   fprintf(stderr, "         -h     : print this message\n");
   fprintf(stderr, "  stdin:\n");
   fprintf(stderr, "         index (int)\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "         quantized vectors (%s)\n", FORMAT);
   fprintf(stderr, "  codebook file:\n");
   fprintf(stderr, "         codebook (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "         -s option are specified number of stages\n");
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
   int l = LENG, *cbsize, *index, stage = 0, ss = 0, num, i;
   FILE *fp = stdin, *fpcb;
   double *x, *cb = NULL, *p;
   char **cbfile;

   cbsize = (int *) calloc(argc / 2, sizeof(*cbsize));
   index = (int *) calloc(argc / 2, sizeof(*index));
   cbfile = (char **) calloc(argc / 2, sizeof(**cbfile));

   p = cb;
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
         case 's':
            cbsize[stage] = atoi(*++argv);
            cbfile[stage++] = *++argv;
            argc -= 2;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   for (i = 0, num = 0; i < stage; i++)
      num += cbsize[i];
   cb = dgetmem(num * l);
   p = cb;

   for (i = 0; i < stage; i++) {
      fpcb = getfp(cbfile[i], "rb");
      if (freadf(p, sizeof(*p), cbsize[i] * l, fpcb) != cbsize[i] * l) {
         fprintf(stderr, "%s : Codebook size error of %d stage!\n", cmnd, ss);
         return (1);
      }
      p += cbsize[i] * l;
   }

   x = dgetmem(l);

   while (freadx(index, sizeof(*index), stage, fp) == stage) {
      imsvq(index, cb, l, cbsize, stage, x);
      fwritef(x, sizeof(*x), l, stdout);
   }

   return (0);
}
