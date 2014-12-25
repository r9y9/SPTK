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
*    evaluate SNR and segmental SNR                                     *
*                                                                       *
*                                        1990.11 T.Kanno                *
*                                        1996.3  K.Koishida             *
*                                                                       *
*       usage:                                                          *
*               snr [ options ] file1 [ infile ] > stdout               *
*       options:                                                        *
*               -l l     :  frame length                         [256]  *
*               -o o     :  output type                          [0]    *
*                             0 SNR and SNRseg           (ascii)        *
*                             1 SNR and SNRseg in detail (ascii)        *
*                             2 SNR                      (float)        *
*                             3 SNRseg                   (float)        *
*      infile:                                                          *
*               data sequence                                           *
*                       , x(0), x(1), ...,                              *
*                       , y(0), y(1), ...,                              *
*      stdout:                                                          *
*               SNR,  SNRseg                                            *
************************************************************************/

static char *rcs_id = "$Id: snr.c,v 1.29 2014/12/11 08:30:49 uratec Exp $";


/*  Standard C Libraries  */
#include <stdio.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define LENG  256
#define OTYPE 0


/*  Command Name  */
char *cmnd;

#ifdef DOUBLE
char *FORMAT1 = "double";
#else
char *FORMAT1 = "float";
#endif                          /* DOUBLE */

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - evaluate SNR and segmental SNR\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] file1 [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "      -l l  : frame length                         [%d]\n",
           LENG);
   fprintf(stderr, "      -o o  : output type                          [%d]\n",
           OTYPE);
   fprintf(stderr, "                0 SNR and SNRseg           (ascii)\n");
   fprintf(stderr, "                1 SNR and SNRseg in detail (ascii)\n");
   fprintf(stderr, "                2 SNR                      (float)\n");
   fprintf(stderr, "                3 SNRseg                   (float)\n");
   fprintf(stderr, "      -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "      data sequence (%s)                        [stdin]\n",
           FORMAT1);
   fprintf(stderr, "  file1:\n");
   fprintf(stderr, "      data sequence\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "      SNR, SNRseg\n");
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
   int l = LENG, otype = OTYPE, count, countold, countseg, xn, yn, size, i;
   FILE *fp2 = stdin, *fp1 = NULL;
   double *x, *y, snr, snrseg, pw1, pw2, pw1snr, pw2snr, sub;
   char *s, c;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 'l':
            l = atoi(*++argv);
            --argc;
            break;
         case 'o':
            otype = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (fp1 == NULL)
         fp1 = getfp(*argv, "rb");
      else
         fp2 = getfp(*argv, "rb");

   x = dgetmem(l + l);
   y = x + l;

   pw1 = pw2 = snrseg = 0.0;
   count = countold = countseg = 0;

   if (otype == 1)
      printf("\n");

   while (!feof(fp1) && !feof(fp2)) {
      xn = freadf(x, sizeof(*x), l, fp1);
      yn = freadf(y, sizeof(*y), l, fp2);
      size = (xn > yn) ? yn : xn;
      pw1snr = pw2snr = 0.0;

      for (i = 0; i < size; i++) {
         pw1snr += x[i] * x[i];
         sub = x[i] - y[i];
         pw2snr += sub * sub;
      }

      count += size;

      if (size) {
         snr = 10.0 * log10(pw1snr / pw2snr);
         if (otype == 1)
            printf("\t %8d   ~ %8d    : %f [dB]\n", countold, count, snr);
      }

      if (size == l) {
         countseg++;
         snrseg += snr;
      }

      pw1 += pw1snr;
      pw2 += pw2snr;
      countold = count;
   }
   snr = 10.0 * log10(pw1 / pw2);
   snrseg /= (double) countseg;

   if (otype == 0 || otype == 1) {
      printf("\n");
      printf("%d samples\n", count);
      printf("  SNR      :  %f [dB]\n", snr);
      printf("  SNRseg   :  %f [dB]\n", snrseg);
      printf("\n");
   }

   if (otype == 2)
      fwritef(&snr, sizeof(snr), 1, stdout);
   if (otype == 3)
      fwritef(&snrseg, sizeof(snrseg), 1, stdout);

   return (0);
}
