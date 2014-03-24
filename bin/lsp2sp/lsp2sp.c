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
*    Transform  LSP to Spectrum                                         *
*                                                                       *
*                                       2013.6 T.Aritake                *
*                                                                       *
*       usage:                                                          *
*               lsp2sp [ options ] [ infile ] > stdout                  *
*       options:                                                        *
*               -m m     :  order of LSP                [25]            *
*               -s s     :  sampling frequency          [10]            *
*               -l l     :  frame length                [256]           *
*               -L       :  regard input log gain as linear one  [FALSE]*
*               -k       :  input gain                  [TRUE]          *
*               -i i     :  input format                [0]             *
*                             0 (normalized frequency <0...pi)>         *
*                             1 (normalized frequency <0...0.5>)        *
*                             2 (frequency (kHz))                       *
*                             3 (frequency (Hz))                        *
*               -o o     :  output format               [0]             *
*                             0 (20 * log|H(z)|)                        *
*                             1 (ln|H(z)|)                              *
*                             2 (|H(z)|)                                *
*                             3 (|H(z)|^2)                              *
*       infile:                                                         *
*               LSP                                                     *
*                   , f(1), ..., f(m),                                  *
*       stdout:                                                         *      
*               spectrum                                                *
*                   , s(0), s(1), ..., s(L/2),                          *
*       require:                                                        *
*               lsp2sp()                                                *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: lsp2sp.c,v 1.2 2013/12/16 09:01:59 mataki Exp $";


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

#include <ctype.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define ORDER 10
#define ITYPE 0
#define SAMPLING 10.0
#define GAIN 1
#define LENG 256
#define OTYPE 0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transform LSP to spectrum\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m  : order of LSP                          [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -s s  : sampling frequency                    [%.1f]\n",
           SAMPLING);
   fprintf(stderr,
           "       -l l  : frame length                          [%d]\n", LENG);
   fprintf(stderr,
           "       -L    : regard input log gain as linear one   [FALSE]\n");
   fprintf(stderr,
           "       -k    : input gain                            [TRUE]\n");
   fprintf(stderr,
           "       -i i  : input format                          [%d]\n",
           ITYPE);
   fprintf(stderr, "                 0 (normalized frequency <0...pi>)\n");
   fprintf(stderr, "                 1 (normalized frequency <0...0.5>)\n");
   fprintf(stderr, "                 2 (frequency (kHz))\n");
   fprintf(stderr, "                 3 (frequency (Hz))\n");
   fprintf(stderr,
           "       -o o  : output format                         [%d]\n",
           OTYPE);
   fprintf(stderr, "                 0 (20*log|H(z)|)\n");
   fprintf(stderr, "                 1 (ln|H(z)|)\n");
   fprintf(stderr, "                 2 (|H(z)|)\n");
   fprintf(stderr, "                 3 (|H(z)|^2)\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       LSP (%s)             [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       spectrum (%s)\n", FORMAT);
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
   int leng = LENG, m = ORDER, itype = ITYPE, otype = OTYPE, gain = GAIN;
   FILE *fp = stdin;
   double logk, sampling = SAMPLING;
   double *x, *lsp;
   int i, no, loggain = 1;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':
         case 's':
         case 'l':
         case 'i':
         case 'o':
            if (isdigit(**(argv + 1)) == 0) {
               if ((**(argv + 1)) != '+') {
                  fprintf(stderr,
                          "%s : %s option need positive value !\n", cmnd,
                          *argv);
                  usage(1);
               } else if (isdigit(*(*(argv + 1) + 1)) == 0) {
                  fprintf(stderr,
                          "%s : %s option need positive value !\n", cmnd,
                          *argv);
                  usage(1);
               }
            }

            if ((*(*argv + 1)) == 'm')
               m = atoi(*++argv);
            else if ((*(*argv + 1)) == 's')
               sampling = atof(*++argv);
            else if ((*(*argv + 1)) == 'l')
               leng = atoi(*++argv);
            else if ((*(*argv + 1)) == 'i')
               itype = atoi(*++argv);
            else if ((*(*argv + 1)) == 'o')
               otype = atoi(*++argv);
            --argc;
            break;
         case 'L':
            loggain = 0;
            break;
         case 'k':
            gain = 0;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   lsp = dgetmem(m + gain);
   no = leng / 2 + 1;
   x = dgetmem(no);
   logk = 20.0 / log(10.0);

   while (freadf(lsp, sizeof(*lsp), m + gain, fp) == m + gain) {

      if (itype == 1)
         for (i = gain; i < m + gain; i++)
            lsp[i] *= PI2;
      else if (itype == 2 || itype == 3) {
         for (i = gain; i < m + gain; i++)
            lsp[i] = lsp[i] / sampling * PI2;
      }

      if (loggain == 0)
         *lsp = log(*lsp);

      lsp2sp(lsp, m, x, no, gain);

      switch (otype) {
      case 1:
         break;
      case 2:
         for (i = no; i--;)
            x[i] = exp(x[i]);
         break;
      case 3:
         for (i = no; i--;)
            x[i] = exp(2 * x[i]);
         break;
      default:
         for (i = no; i--;)
            x[i] *= logk;
         break;
      }
      fwritef(x, sizeof(*x), no, stdout);
   }

   return (0);
}
