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
*    Check stability and rearrange LSP                                  *
*                                                                       *
*                                         1996.6  K.Koishida            *
*                                         2009.9  A.Saito               *
*       usage:                                                          *
*               lspcheck [ options ] [ infile ] > stdout                *
*       options:                                                        *
*               -m m     :  order of LSP                [25]            *
*               -s s     :  sampling frequency          [10]            *
*               -k       :  input & output gain         [FALSE]         *
*               -q q     :  input format                [0]             *
*               -o o     :  output format               [i]             *
*                             0 (normalized frequency <0...pi>)         *
*                             1 (normalized frequency <0...0.5>)        *
*                             2 (frequency (kHz))                       *
*                             3 (frequency (Hz))                        *
*               -c       :  rearrange LSP               [N/A]           *
*                           distance between two consecutive LSPs       *
*                           extend the distance (if it is smaller       *
*                           than R*pi/m)    s.t. (0 <= R <= 1)          *
*               -r R     :  threshold of rearrangement  [0.0]           *
*               -g       :  modify gain value           [N/A]           *
*                           check whether gain is less than G or not    *
*               -L       :  regard input as log gain    [N/A]           *
*               -G G     :  min. value of gain s.t. G > 0 [1.0E-10]     *
*       infile:                                                         *
*               LSP                                                     *
*                       , f(1), ..., f(m),                              *
*       stdout:                                                         *
*               LSP or rearranged LSP if -r option is specified         *
*       stderr:                                                         *
*               frame number of irregular LSP                           *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: lspcheck.c,v 1.41 2014/12/25 02:41:38 uratec Exp $";


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
#define ORDER  25
#define ITYPE  0
#define OTYPE  -1
#define SAMPLING 10.0
#define ARRANGE  0
#define GAIN  TR
#define LOGGAIN  FA
#define GAIN_MIN 1.0E-10
#define ALPHA 0.0

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - check stability and rearrange LSP\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m  : order of LPC                               [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -s s  : sampling frequency                         [%g]\n",
           SAMPLING);
   fprintf(stderr,
           "       -k    : input & output gain                        [%s]\n",
           BOOL[GAIN]);
   fprintf(stderr,
           "       -q q  : input format                               [%d]\n",
           ITYPE);
   fprintf(stderr,
           "       -o o  : output format                              [i]\n");
   fprintf(stderr, "                 0 (normalized frequency <0...pi>)\n");
   fprintf(stderr, "                 1 (normalized frequency <0...0.5>)\n");
   fprintf(stderr, "                 2 (frequency (kHz))\n");
   fprintf(stderr, "                 3 (frequency (Hz))\n");
   fprintf(stderr,
           "       -c    : rearrange LSP                              [N/A]\n");
   fprintf(stderr, "               check the distance between two\n");
   fprintf(stderr, "               consecutive LSPs and extend the distance\n");
   fprintf(stderr, "               (if it is smaller than R*pi/m)\n");
   fprintf(stderr,
           "       -r R  : threshold of rearrangement of LSP          [%.1f]\n",
           ALPHA);
   fprintf(stderr, "               s.t. 0 <= R <= 1\n");
   fprintf(stderr,
           "       -g    : modify gain value if gain is less than G   [N/A]\n");
   fprintf(stderr,
           "       -L    : regard input as log gain                   [%s]\n",
           BOOL[LOGGAIN]);
   fprintf(stderr,
           "       -G G  : min. value of gain                         [%g]\n",
           GAIN_MIN);
   fprintf(stderr, "               s.t. G > 0\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       LSP (%s)                                        [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr,
           "       LSP (%s) or rearranged LSP (%s) if -r option \n",
           FORMAT, FORMAT);
   fprintf(stderr, "       is specified\n");
   fprintf(stderr, "  stderr:\n");
   fprintf(stderr, "       irregular LSP and its frame number\n");
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
   int m = ORDER, itype = ITYPE, otype = OTYPE, i, num;
   Boolean arrange = ARRANGE, gain = GAIN, modify_gain = FA, loggain = LOGGAIN;
   FILE *fp = stdin;
   double *lsp, alpha = ALPHA, sampling = SAMPLING, min, gain_min = GAIN_MIN;
   void lsparrange(double *lsp, const int ord, double min);

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
         case 'i':
         case 'q':
            itype = atoi(*++argv);
            --argc;
            break;
         case 'o':
            otype = atoi(*++argv);
            --argc;
            break;
         case 'r':
            alpha = atof(*++argv);
            if (alpha < 0 || alpha > 1) {
               fprintf(stderr, "%s : Invalid option 'r'!\n", cmnd);
               usage(0);
            }
            --argc;
            break;
         case 'k':
            gain = 1 - gain;
            break;
         case 'l':
         case 'L':
            loggain = 1 - loggain;
            break;
         case 'g':
            modify_gain = 1 - modify_gain;
            break;
         case 'G':
            gain_min = atof(*++argv);
            --argc;
            break;
         case 'c':
            arrange = 1 - arrange;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else {
         fp = getfp(*argv, "rb");
      }
   if (otype < 0)
      otype = itype;

   lsp = dgetmem(m + gain);
   num = 0;
   min = alpha * PI / m;

   switch (itype) {
   case 0:
      min /= PI2;
      break;
   case 1:
      break;
   case 2:
      min /= sampling;
      break;
   case 3:
      min /= (sampling * 1000);
      break;
   }

   while (freadf(lsp, sizeof(*lsp), m + gain, fp) == m + (int) gain) {
      if (gain) {
         if (loggain) {
            if (lsp[0] < log(gain_min)) {
               fprintf(stderr, "[No. %d] log gain %g is less than %g\n",
                       num, lsp[0], log(gain_min));
               if (modify_gain) {
                  lsp[0] = log(gain_min);
               }
            }
         } else {
            if (lsp[0] < gain_min) {
               fprintf(stderr, "[No. %d] gain %g is less than %g\n",
                       num, lsp[0], gain_min);
               if (modify_gain) {
                  lsp[0] = gain_min;
               }
            }
         }
      }
      switch (itype) {
      case 0:
         for (i = gain; i < m + (int) gain; i++)
            lsp[i] /= PI2;
         break;
      case 1:
         break;
      case 2:
         for (i = gain; i < m + (int) gain; i++)
            lsp[i] /= sampling;
         break;
      case 3:
         for (i = gain; i < m + (int) gain; i++)
            lsp[i] /= (sampling * 1000);
         break;
      }

      if (lspcheck(lsp + gain, m) == -1) {      /* unstable */
         fprintf(stderr, "[No. %d] is unstable frame\n", num);
      }
      if (arrange) {
         lsparrange(lsp + gain, m, min);
      }

      switch (otype) {
      case 0:
         for (i = gain; i < m + (int) gain; i++)
            lsp[i] *= PI2;
         break;
      case 1:
         break;
      case 2:
         for (i = gain; i < m + (int) gain; i++)
            lsp[i] *= sampling;
         break;
      case 3:
         for (i = gain; i < m + (int) gain; i++)
            lsp[i] *= (sampling * 1000);
         break;
      }

      fwritef(lsp, sizeof(*lsp), m + gain, stdout);
      num++;
   }

   return (0);
}
