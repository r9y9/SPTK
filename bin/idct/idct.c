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
*    IDCT : Inverse Discrete Cosine Transform                           *
*                                                                       *
*                                             2011.4 Akira Tamamori     *
*                                                                       *
*       usage:                                                          *
*               idct [ options ] [ infile ] > stdout                    *
*       options:                                                        *
*               -l l     :  IDCT size                    [256]          *
*               -c       :  use complex number           [FALSE]        *
*               -d       :  without using FFT algorithm  [FALSE]        *
*       infile:                                                         *
*               stdin for default                                       *
*       stdout:                                                         *
*               IDCT data sequence                                      *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: idct.c,v 1.10 2014/12/11 08:30:37 uratec Exp $";

/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

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
#define SIZE    256
#define DFTMODE FA
#define COMPLEX FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;

/* workspace */
static int dct_table_size = 0;
static double *dct_workspace = NULL;
static double *pLocalReal = NULL;
static double *pLocalImag = NULL;
static double *pWeightReal = NULL;
static double *pWeightImag = NULL;

Boolean dftmode = DFTMODE;
int size = SIZE;

int usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - Inverse Discrete Cosine Transform\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -l l  : IDCT size                              [%d]\n",
           SIZE);
   fprintf(stderr,
           "       -c    : use comlex number                      [FALSE]\n");
   fprintf(stderr,
           "       -d    : without using FFT algorithm (with DFT) [%s]\n",
           BOOL[DFTMODE]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)                     [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       IDCT sequence (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(1);
}

int idct_create_table(const int nSize)
{
   int k;

   if (nSize == dct_table_size) {
      /* no need to resize workspace */
      return (0);
   } else {
      /* release resources to resize workspace */
      if (dct_workspace != NULL) {
         free(dct_workspace);
         dct_workspace = NULL;
      }
      pLocalReal = NULL;
      pLocalImag = NULL;
      pWeightReal = NULL;
      pWeightImag = NULL;
   }

   if (nSize <= 0) {
      dct_table_size = 0;
      fprintf(stderr, "DCT size must be positive!\n");
      return (-1);
   } else {
      dct_table_size = nSize;
      dct_workspace = dgetmem(dct_table_size * 8);
      pWeightReal = dct_workspace;
      pWeightImag = dct_workspace + 2 * dct_table_size;
      pLocalReal = dct_workspace + (4 * dct_table_size);
      pLocalImag = dct_workspace + (6 * dct_table_size);

      for (k = 0; k < 2 * dct_table_size; k++) {
         pWeightReal[k] = cos(k * PI / (2.0 * dct_table_size))
             / sqrt(2.0 * dct_table_size);
         pWeightImag[k] = -sin(k * PI / (2.0 * dct_table_size))
             / sqrt(2.0 * dct_table_size);
      }
      pWeightReal[0] *= sqrt(2.0);
      pWeightImag[0] *= sqrt(2.0);
   }

   return (0);
}

int idct(double *pReal, double *pImag,
         const double *pInReal, const double *pInImag, int PowerOf2)
{
   int k, n;
   double rtemp, itemp;

   for (n = 1; n < dct_table_size; n++) {
      pLocalReal[n] = pInReal[n];
      pLocalImag[n] = pInImag[n];
      pLocalReal[dct_table_size + n] = -pInReal[dct_table_size - n];
      pLocalImag[dct_table_size + n] = -pInImag[dct_table_size - n];
   }
   pLocalReal[0] = pInReal[0];
   pLocalImag[0] = pInImag[0];
   pLocalReal[dct_table_size] = 0.0;
   pLocalImag[dct_table_size] = 0.0;

   /* 1/2 sample shift in the temporal domain */
   for (k = 0; k < 2 * dct_table_size; k++) {
      rtemp = pLocalReal[k];
      itemp = pLocalImag[k];
      pLocalReal[k] = rtemp * pWeightReal[k] - itemp * pWeightImag[k];
      pLocalImag[k] = rtemp * pWeightImag[k] + itemp * pWeightReal[k];
   }

   if (size != PowerOf2 || dftmode) {
      dft(pLocalReal, pLocalImag, dct_table_size * 2);
   } else {
      fft(pLocalReal, pLocalImag, dct_table_size * 2);
   }

   for (k = 0; k < dct_table_size; k++) {
      pReal[k] = pLocalReal[k];
      pImag[k] = pLocalImag[k];
   }

   return (0);
}

int main(int argc, char *argv[])
{
   char *s, *infile = NULL, c;
   int i, j, iter, size2;
   double *x, *y, *pReal, *pImag;
   FILE *fp;
   Boolean comp = COMPLEX;

   if ((cmnd = strrchr(argv[0], '/')) == NULL) {
      cmnd = argv[0];
   } else {
      cmnd++;
   }

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if ((c == 'l') && (*++s == '\0')) {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'l':             /* IDCT size */
            size = atoi(s);
            break;
         case 'c':             /* use complex number */
            comp = 1 - COMPLEX;
            break;
         case 'd':             /* IDCT without FFT (DFT) */
            dftmode = 1 - dftmode;
            break;
         case 'h':
         default:
            usage();
         }
      } else {
         infile = s;
      }
   }
   if (infile) {
      fp = getfp(infile, "rb");
   } else {
      fp = stdin;
   }

   /* memory allocation */
   x = dgetmem(size2 = size + size);
   y = x + size;
   pReal = dgetmem(size2);
   pImag = pReal + size;

   while (!feof(fp)) {
      fillz(x, size2, sizeof(*x));
      if (freadf(x, sizeof(*x), size, fp) == 0) {
         break;
      }
      if (comp) {
         if (freadf(y, sizeof(*y), size, fp) == 0) {
            break;
         }
      }

      /* decide wheather 'size' is a power of 2 or not */
      for (iter = 0, i = size; (i /= 2) != 0; iter++);
      for (i = 1, j = 1; i <= iter; i++, j *= 2);

      /* IDCT-II (DCT-III) routine */
      idct_create_table(size);
      idct(pReal, pImag, (const double *) x, (const double *) y, j);

      /* output IDCT sequence */
      fwritef(pReal, sizeof(*pReal), size, stdout);
      if (comp) {
         fwritef(pImag, sizeof(*pImag), size, stdout);
      }
   }

   if (infile) {
      fclose(fp);
   }

   return (0);
}
