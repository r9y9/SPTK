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
*    All Zero Digital Filter for Speech Synthesis                       *
*                                                                       *
*                                       1996.3  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               zerodf [ options ] bfile [ infile ] > stdout            *
*       options:                                                        *
*               -m m     :  order of coefficients  [25]                 *
*               -p p     :  frame period           [100]                *
*               -i i     :  interpolation period   [1]                  *
*               -t       :  transpose filter       [FALSE]              *
*               -k       :  filtering without gain [FALSE]              *
*       infile:                                                         *
*               coefficients                                            * 
*                       , b(0), b(1), ..., b(m),                        *
*               excitation sequence                                     *
*                       , x(0), x(1), ...,                              *
*       stdout:                                                         *
*               filtered sequence                                       *
*                       , y(0), y(1), ...,                              *
*       require:                                                        *
*               zerodf(), zerodft()                                     *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: zerodf.c,v 1.24 2014/12/11 08:30:52 uratec Exp $";


/*  Standard C Libralies  */
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

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define ORDER     25
#define FPERIOD   100
#define IPERIOD   1
#define TRANSPOSE FA
#define NGAIN     FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - all zero digital filter for speech synthesis\n",
           cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] bfile [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of coefficients  [%d]\n", ORDER);
   fprintf(stderr, "       -p p  : frame period           [%d]\n", FPERIOD);
   fprintf(stderr, "       -i i  : interpolation period   [%d]\n", IPERIOD);
   fprintf(stderr, "       -t    : transpose filter       [%s]\n",
           BOOL[TRANSPOSE]);
   fprintf(stderr, "       -k    : filtering without gain [%s]\n", BOOL[NGAIN]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       filter input (%s)           [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       filter output (%s)\n", FORMAT);
   fprintf(stderr, "  bfile:\n");
   fprintf(stderr, "       MA coefficients (%s)\n", FORMAT);
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
   int m = ORDER, fprd = FPERIOD, iprd = IPERIOD, i, j;
   FILE *fp = stdin, *fpc = NULL;
   double *c, *inc, *cc, *d, x;
   Boolean tp = TRANSPOSE, ngain = NGAIN;

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
         case 'p':
            fprd = atoi(*++argv);
            --argc;
            break;
         case 'i':
            iprd = atoi(*++argv);
            --argc;
            break;
         case 't':
            tp = 1 - tp;
            break;
         case 'k':
            ngain = 1 - ngain;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (fpc == NULL)
         fpc = getfp(*argv, "rb");
      else
         fp = getfp(*argv, "rb");

   if (fpc == NULL) {
      fprintf(stderr, "%s : Cannot open cepstrum file!\n", cmnd);
      return (1);
   }

   c = dgetmem(m + m + m + 3 + m);
   cc = c + m + 1;
   inc = cc + m + 1;
   d = inc + m + 1;

   if (freadf(c, sizeof(*c), m + 1, fpc) != m + 1)
      return (1);

   for (;;) {
      if (freadf(cc, sizeof(*cc), m + 1, fpc) != m + 1)
         return (0);

      for (i = 0; i <= m; i++)
         inc[i] = (cc[i] - c[i]) * iprd / fprd;

      for (j = fprd, i = (iprd + 1) / 2; j--;) {
         if (freadf(&x, sizeof(x), 1, fp) != 1)
            return (0);

         if (ngain)
            for (i = m; i >= 0; i--)
               c[i] /= c[0];

         if (!tp)
            x = zerodf(x, c, m, d);
         else
            x = zerodft(x, c, m, d);

         fwritef(&x, sizeof(x), 1, stdout);

         if (!--i) {
            for (i = 0; i <= m; i++)
               c[i] += inc[i];
            i = iprd;
         }
      }
      movem(cc, c, sizeof(*cc), m + 1);
   }
   return (0);
}
