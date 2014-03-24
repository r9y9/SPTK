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
*    Transform LSP to LPC                                               *
*                                                                       *
*                                       1996.1  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               lsp2lpc [ options ] [ infile ] > stdout                 *
*       options:                                                        *
*               -m m     :  order of LPC                [25]            *
*               -s s     :  sampling frequency          [10]            *
*               -k       :  input & output gain         [TRUE]          *
*               -L       :  regard input as log gain    [FALSE]         *
*                           and output linear one                       *
*               -i i     :  input format                [0]             *
*                             0 (normalized frequency <0...pi>)         *
*                             1 (normalized frequency <0...0.5>)        *
*                             2 (frequency (kHz))                       *
*                             3 (frequency (Hz))                        *
*       infile:                                                         *
*               LSP                                                     *
*                   , f(1), ..., f(m),                                  *
*       stdout:                                                         *
*               LPC                                                     *
*                   , K(=1), a(1), ..., a(M),                           *
*       require:                                                        *
*               lsp2lpc()                                               *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: lsp2lpc.c,v 1.32 2013/12/16 09:01:59 mataki Exp $";


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

#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define ORDER    25
#define ITYPE    0
#define SAMPLING 10.0
#define GAIN     1
#define LOGGAIN  FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transform LSP to LPC\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ]>stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m  : order of LPC                                   [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -s s  : sampling frequency                             [%g]\n",
           SAMPLING);
   fprintf(stderr,
           "       -k    : input & output gain                            [TRUE]\n");
   fprintf(stderr,
           "       -L    : regard input as log gain and output linear one [%s]\n",
           BOOL[LOGGAIN]);
   fprintf(stderr,
           "       -i i  : input format                                   [%d]\n",
           ITYPE);
   fprintf(stderr, "                 0 (normalized frequency <0...pi>)\n");
   fprintf(stderr, "                 1 (normalized frequency <0...0.5>)\n");
   fprintf(stderr, "                 2 (frequency (kHz))\n");
   fprintf(stderr, "                 3 (frequency (Hz))\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       LSP (%s)                                            [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       LP coefficients (%s)\n", FORMAT);
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
   int m = ORDER, itype = ITYPE, i, gain = GAIN;
   FILE *fp = stdin;
   double *a, *lsp, sampling = SAMPLING;
   Boolean loggain = LOGGAIN;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':
            m = atoi(*++argv);
            --argc;
            break;
         case 's':
            sampling = atof(*++argv);
            --argc;
            break;
         case 'k':
            gain = 0;
            break;
         case 'l':
         case 'L':
            loggain = TR;
            break;
         case 'i':
            itype = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   lsp = dgetmem(m + m + 1 + gain);
   a = lsp + m + gain;

   while (freadf(lsp, sizeof(*lsp), m + gain, fp) == m + gain) {
      if (itype == 0)
         for (i = gain; i < m + gain; i++)
            lsp[i] /= PI2;
      else if (itype == 2 || itype == 3)
         for (i = gain; i < m + gain; i++)
            lsp[i] /= sampling;

      if (itype == 3)
         for (i = gain; i < m + gain; i++)
            lsp[i] /= 1000;

      lsp2lpc(lsp + gain, a, m);

      if (gain) {
         if (loggain)
            *lsp = exp(*lsp);
         fwritef(lsp, sizeof(*lsp), 1, stdout);
      }
      fwritef(a + gain, sizeof(*a), m + 1 - gain, stdout);
   }

   return (0);
}
