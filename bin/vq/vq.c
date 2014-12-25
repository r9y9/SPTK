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
*     Vector Quantization                                               *
*                                                                       *
*                                           1995.12  K.Koishida         *
*                                                                       *
*        usage:                                                         *
*                vq [ options ] cbfile [ infile ] > stdout              *
*        options:                                                       *
*                -l l :  length of vector           [26]                *   
*                -n n :  order of vector            [25]                *
*                -q   :  output quantized vector    [FALSE]             *
*        infile:                                                        *
*                input vector                                           *
*                        x(0), x(1), ...                                *
*                codebook vector                                        *
*                        cb(0), cb(1), ... cb(l*c-1)                    *
*        stdout:                                                        *
*                index of codebook (int)                                *
*                        index(0), index(1), ...,                       *
*                quantized vector (if -q option is specified)           *
*                        x'(0), x'(1), ...                              *
*       require:                                                        *
*                vq()                                                   *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: vq.c,v 1.23 2014/12/11 08:30:51 uratec Exp $";


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
#define LENG   26
#define CBSIZE 256
#define QFLAG  FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - vector quantization\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] cbfile [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : length of vector        [%d]\n", LENG);
   fprintf(stderr, "       -n n  : order of vector         [%d]\n", LENG - 1);
   fprintf(stderr, "       -q    : output quantized vector [%s]\n",
           BOOL[QFLAG]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       vectors (%s)                 [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       index (int) or\n");
   fprintf(stderr, "       quantized vectors (%s) if -q option is specified\n",
           FORMAT);
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
   Boolean qflag = QFLAG;
   FILE *fp = stdin, *fpcb = NULL;
   double *x, *qx, *cb;

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
         case 'q':
            qflag = 1 - qflag;
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

   x = dgetmem(l + l + cbsize * l);
   qx = x + l;
   cb = qx + l;

   if (freadf(cb, sizeof(*cb), cbsize * l, fpcb) != cbsize * l) {
      fprintf(stderr, "%s : Codebook size error!\n", cmnd);
      return (1);
   }

   if (!qflag)
      while (freadf(x, sizeof(*x), l, fp) == l) {
         index = vq(x, cb, l, cbsize);
         fwritex(&index, sizeof(index), 1, stdout);
   } else
      while (freadf(x, sizeof(*x), l, fp) == l) {
         index = vq(x, cb, l, cbsize);
         ivq(index, cb, l, qx);
         fwritef(qx, sizeof(*qx), l, stdout);
      }

   return (0);
}
