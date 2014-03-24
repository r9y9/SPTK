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
*    Frequency Transformation                                           *
*                                                                       *
*                                       1988.2  K.Tokuda                *
*                                       1996.1  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               freqt [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -m m     :  order of minimum phase sequence     [25]    *
*               -M M     :  order of warped sequence            [25]    *
*               -a a     :  all-pass constant of input sequence [0]     *
*               -A A     :  all-pass constant of output sequence[0.35]  *
*       infile:                                                         *
*               minimum phase sequence                                  *
*                   , c(0), c(1), ..., c(M),                            *
*       stdout:                                                         *
*               warped sequence                                         *
*                   , c~(0), c~(1), ..., c~(N),                         *
*       require:                                                        *
*               freqt()                                                 *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: freqt.c,v 1.22 2013/12/16 09:01:57 mataki Exp $";


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
#define ORDERC1 25
#define ORDERC2 25
#define ALPHA1 0.0
#define ALPHA2 0.35

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - frequency transformation\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of minimum phase sequence      [%d]\n",
           ORDERC1);
   fprintf(stderr, "       -M M  : order of warped sequence             [%d]\n",
           ORDERC2);
   fprintf(stderr, "       -a a  : all-pass constant of input sequence  [%g]\n",
           ALPHA1);
   fprintf(stderr, "       -A A  : all-pass constant of output sequence [%g]\n",
           ALPHA2);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       minimum phase sequence (%s)               [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       warped sequence (%s)\n", FORMAT);
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
   int m1 = ORDERC1, m2 = ORDERC2;
   FILE *fp = stdin;
   double *c1, *c2, a1 = ALPHA1, a2 = ALPHA2, a;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':
            m1 = atoi(*++argv);
            --argc;
            break;
         case 'M':
            m2 = atoi(*++argv);
            --argc;
            break;
         case 'a':
            a1 = atof(*++argv);
            --argc;
            break;
         case 'A':
            a2 = atof(*++argv);
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

   c1 = dgetmem(m1 + m2 + 2);
   c2 = c1 + m1 + 1;

   a = (a2 - a1) / (1 - a1 * a2);

   while (freadf(c1, sizeof(*c1), m1 + 1, fp) == m1 + 1) {
      freqt(c1, m1, c2, m2, a);
      fwritef(c2, sizeof(*c2), m2 + 1, stdout);
   }

   return 0;
}
