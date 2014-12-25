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
*    2nd Order Standard Form Digital Filter                             *
*                                                                       *
*                                       1985.8  K.Tokuda                *
*                                       1995.12 N.Isshiki modified      *
*                                                                       *
*       usage:                                                          *
*               df2 [ options ] [ infile ] > stdout                     *
*       options:                                                        *
*               -s s     :  sampling frequency(kHz)       [10.0]        *
*               -p f b   :  center frequency f(Hz)                      *
*                            and band width b(Hz) of pole [N/A]         *
*               -z f b   :  center frequency f(Hz)                      *
*                            and band width b(Hz) of zero [N/A]         *
*       note:                                                           *
*               -p and -z option can be specified repeatedly.           *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: df2.c,v 1.27 2014/12/11 08:30:33 uratec Exp $";

/* Standard C Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
#define SAMPLEF 10.0
#define SIZE 2048

/* Command Name */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - 2nd order standard form digital filter\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout \n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -s s   : sampling frequency (kHz)       [%.1f]\n",
           SAMPLEF);
   fprintf(stderr, "       -p f b : center frequency f(Hz)\n");
   fprintf(stderr, "                 and band width b(Hz) of pole  [N/A]\n");
   fprintf(stderr, "       -z f b : center frequency f(Hz)\n");
   fprintf(stderr, "                 and band width b(Hz) of zero  [N/A]\n");
   fprintf(stderr, "       -h     : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       filter input (%s)                    [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       filter output (%s)\n", FORMAT);
   fprintf(stderr, "  note:\n");
   fprintf(stderr, "       -p and -z option can be specified repeatedly.\n");
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
   FILE *fpp = stdin;
   int bufp[SIZE], fp = 0, fz = 0, i, n, fp1, fz1;
   double x;
   double sf = SAMPLEF, af0[SIZE], bf0[SIZE], ab[SIZE], bb[SIZE];
   double buf[SIZE][3];
   double af01, bf01, ab1, bb1;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         argc--;
         switch (*(*argv + 1)) {
         case 'f':
            sf = atof(*++argv);
            break;
         case 's':
            sf = atof(*++argv);
            break;
         case 'z':
            af0[fz] = atof(*++argv);
            argc--;
            ab[fz] = atof(*++argv);
            fz++;
            break;
         case 'p':
            bf0[fp] = atof(*++argv);
            argc--;
            bb[fp] = atof(*++argv);
            fp++;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fpp = getfp(*argv, "rb");

   sf *= 1000;                  /* kHz -> Hz */

   n = (fp > fz) ? fp : fz;
   for (i = 0; i < n; i++) {
      fillz(buf[i], sizeof(**buf), 3);
      bufp[i] = 0;
   }

   for (;;) {
      if (freadf(&x, sizeof(x), 1, fpp) != 1)
         break;

      for (i = 0; i < n; i++) {
         bf01 = (i >= fp) ? 0 : bf0[i];
         bb1 = (i >= fp) ? 0 : bb[i];
         af01 = (i >= fz) ? 0 : af0[i];
         ab1 = (i >= fz) ? 0 : ab[i];
         fp1 = (i >= fp) ? 0 : 1;
         fz1 = (i >= fz) ? 0 : 1;

         x = df2(x, sf, bf01, bb1, af01, ab1, fp1, fz1, buf[i], &bufp[i]);
      }
      fwritef(&x, sizeof(x), 1, stdout);
   }

   return 0;
}
