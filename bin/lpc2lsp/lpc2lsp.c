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

/*************************************************************************
*                                                                        *
*    Transform LPC to LSP                                                *
*                                                                        *
*                                        1998.11 K.Koishida              *
*                                                                        *
*       usage:                                                           *
*               lpc2lsp [ options ] [ infile ] > stdout                  *
*       options:                                                         *
*               -m m  :  order of LPC                            [25]    *
*               -s s  :  sampling frequency                      [10]    *
*               -k    :  output gain                             [TRUE]  *
*               -L    :  output log gain rather than linear gain [FALSE] *
*               -o o  :  output format                           [0]     *
*                          0 (normalized frequency [0...pi])             *
*                          1 (normalized frequency [0...0.5])            *
*                          2 (frequency [kHz])                           *
*                          3 (frequency [Hz])                            *
*               (level 2)                                                *
*               -n n  :  split number of unit circle             [128]   *
*               -p p  :  maximum number of interpolation         [4]     *
*               -d d  :  end condition of interpolation          [1e-6]  *
*      infile:                                                           *
*               LP coefficients                                          *
*                       , K, a(1), ..., a(m),                            *
*      stdout:                                                           *
*               output format LSP                                        *
*                       0  normalized frequency (0 ~ pi)                 *
*                       1  normalized frequency (0 ~ 0.5)                * 
*                       2  frequency (kHz)                               *
*                       3  frequency (Hz)                                *
*               LSP                                                      *
*                       , f(1), ..., f(m),                               *
*      require:                                                          *
*              lpc2lsp()                                                 *
*                                                                        *
*************************************************************************/

static char *rcs_id = "$Id: lpc2lsp.c,v 1.35 2014/12/11 08:30:39 uratec Exp $";


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
#define SAMPLING 10.0
#define OTYPE    0
#define SPNUM    128
#define MAXITR   4
#define END      1e-6
#define GAIN     TR
#define LOGGAIN  FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transform LPC to LSP\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m  : order of LPC                            [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -s s  : sampling frequency                      [%g]\n",
           SAMPLING);
   fprintf(stderr,
           "       -k    : output gain                             [%s]\n",
           BOOL[GAIN]);
   fprintf(stderr,
           "       -L    : output log gain rather than linear gain [%s]\n",
           BOOL[LOGGAIN]);
   fprintf(stderr,
           "       -o o  : output format                           [%d]\n",
           OTYPE);
   fprintf(stderr, "                 0 (normalized frequency [0...pi])\n");
   fprintf(stderr, "                 1 (normalized frequency [0...0.5])\n");
   fprintf(stderr, "                 2 (frequency [kHz])\n");
   fprintf(stderr, "                 3 (frequency [Hz])\n");
   fprintf(stderr, "     (level 2)\n");
   fprintf(stderr,
           "       -n n  : split number of unit circle             [%d]\n",
           SPNUM);
   fprintf(stderr,
           "       -p p  : maximum number of interpolation         [%d]\n",
           MAXITR);
   fprintf(stderr,
           "       -d d  : end condition of interpolation          [%g]\n",
           END);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       LP coefficients (%s)                         [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       LSP (%s)\n", FORMAT);
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
   int m = ORDER, otype = OTYPE, n = SPNUM, p = MAXITR, i;
   FILE *fp = stdin;
   double *a, *lsp, end = END, sampling = SAMPLING;
   Boolean gain = GAIN, loggain = LOGGAIN;

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
         case 'o':
            otype = atoi(*++argv);
            --argc;
            break;
         case 'p':
            p = atoi(*++argv);
            --argc;
            break;
         case 'n':
            n = atoi(*++argv);
            --argc;
            break;
         case 'd':
            end = atof(*++argv);
            --argc;
            break;
         case 'k':
            gain = 1 - gain;
            break;
         case 'l':
         case 'L':
            loggain = 1 - loggain;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   lsp = dgetmem(m + m + 1);
   a = lsp + m;

   while (freadf(a, sizeof(*a), m + 1, fp) == m + 1) {
      lpc2lsp(a, lsp, m, n, p, end);

      if (otype == 0)
         for (i = 0; i < m; i++)
            lsp[i] *= PI2;
      else if (otype == 2 || otype == 3)
         for (i = 0; i < m; i++)
            lsp[i] *= sampling;
      if (otype == 3)
         for (i = 0; i < m; i++)
            lsp[i] *= 1000;

      if (gain) {
         if (loggain)
            *a = log(*a);
         fwritef(a, sizeof(*a), 1, stdout);
      }
      fwritef(lsp, sizeof(*lsp), m, stdout);
   }

   return (0);
}
