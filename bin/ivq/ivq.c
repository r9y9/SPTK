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
*     Decoder of Vector Quantization                                    *
*                                                                       *
*                                                1996.1  K.Koishida     *
*                                                                       *
*        usage:                                                         *
*                ivq [ options ] cbfile [ infile ] > stdout             *
*        options:                                                       *
*                -l l      :  length of vector      [26]                *
*                -n n      :  order of vector       [25]                *
*        infile:                                                        *
*                codebook index (int)                                   *
*        stdout:                                                        *
*                quantized vector                                       *
*                         x'(0), x'(1), ..., x'(l-1),                   *
*        require:                                                       *
*                ivq()                                                  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: ivq.c,v 1.23 2014/12/11 08:30:38 uratec Exp $";


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
#define LENG  26
#define CBSIZE  256

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - decoder of vector quantization \n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] cbfile [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : length of vector   [%d]\n", LENG);
   fprintf(stderr, "       -n n  : order of vector    [%d]\n", LENG - 1);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       index (int)                [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       quantized vector (%s)\n", FORMAT);
   fprintf(stderr, "  cbfile:\n");
   fprintf(stderr, "       codebook (%s)\n", FORMAT);
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
   int l = LENG, cbsize = CBSIZE, index;
   FILE *fp = stdin, *fpcb = NULL;
   double *x, *cb;

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
            cbsize = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (fpcb == NULL)
         fpcb = getfp(*argv, "rb");
      else
         fp = getfp(*argv, "rb");

   fseek(fpcb, 0, 2);
#ifdef DOUBLE
   cbsize = ftell(fpcb) / sizeof(double) / l;
#else
   cbsize = ftell(fpcb) / sizeof(float) / l;
#endif                          /* DOUBLE */
   rewind(fpcb);

   x = dgetmem(l + cbsize * l);
   cb = x + l;

   if (freadf(cb, sizeof(*cb), cbsize * l, fpcb) != cbsize * l) {
      fprintf(stderr, "%s : Codebook size error!\n", cmnd);
      return (1);
   }

   while (freadx(&index, sizeof(index), 1, fp) == 1) {
      ivq(index, cb, l, x);
      fwritef(x, sizeof(*x), l, stdout);
   }

   return (0);
}
