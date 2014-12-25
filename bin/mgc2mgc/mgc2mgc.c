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
*    Frequency and Generalized Cepstral Transformation                   *
*                                                                        *
*                                        1996.1  K.Koishida              *
*                                                                        *
*       usage:                                                           *
*               mgc2mgc [ options ] [ infile ] > stdout                  *
*       options:                                                         *
*               -m  m    : order of generalized cepstrum (input)  [25]   *
*               -a  a    : alpha of generalized cepstrum (input)  [0]    *
*               -g  g    : gamma of generalized cepstrum (input)  [0]    *
*               -c  c    : gamma of generalized cepstrum (input)         *
*                          gamma = -1 / (int) c                          *
*               -n       : regard input as normalized                    *
*                          mel-generalized cepstrum               [FALSE]*
*               -u       : regard input as multiplied by gamma    [FALSE]*
*               -M  M    : order of generalized cepstrum (output) [25]   *
*               -A  A    : alpha of generalized cepstrum (output) [0]    *
*               -G  G    : gamma of generalized cepstrum (output) [1]    *
*               -C  C    : gamma of generalized cepstrum (output)        *
*                          gamma = -1 / (int) C                          *
*               -N  N    : regard output as normalized                   *
*                          mel-generalized cepstrum               [FALSE]*
*               -U  : regard output as multiplied by gamma        [FALSE]*
*       infile:                                                          *
*               mel-generalized cepstrum                                 *
*                       , c(0), c(1), ..., c(m),                         *
*       stdout:                                                          *
*               mel-generalized cepstrum                                 *
*                       , c'(0)(=c(0)), c'(1), ..., c'(M),               *
*       notice:                                                          *    
*               value of c and C must be c>=1, C>=1                      *
*       require:                                                         *
*               mgc2mgc(), gnorm(), ignorm()                             *
*                                                                        *
*************************************************************************/

static char *rcs_id = "$Id: mgc2mgc.c,v 1.31 2014/12/11 08:30:41 uratec Exp $";


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
#define ORDER1 25
#define ORDER2 25
#define ALPHA1 0.0
#define ALPHA2 0.0
#define GAMMA1 0.0
#define GAMMA2 1.0
#define NORMFLG1 FA
#define NORMFLG2 FA
#define MULGFLG1 FA
#define MULGFLG2 FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - frequency and generalized cepstral transformation\n",
           cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m  : order of mel-generalized cepstrum (input)            [%d]\n",
           ORDER1);
   fprintf(stderr,
           "       -a a  : alpha of mel-generalized cepstrum (input)            [%g]\n",
           ALPHA1);
   fprintf(stderr,
           "       -g g  : gamma of mel-generalized cepstrum (input)            [%g]\n",
           GAMMA1);
   fprintf(stderr,
           "       -c c  : gamma of mel-generalized cepstrum = -1 / (int) c (input) \n");
   fprintf(stderr,
           "       -n    : regard input as normalized mel-generalized cepstrum  [%s]\n",
           BOOL[NORMFLG1]);
   fprintf(stderr,
           "       -u    : regard input as multiplied by gamma                  [%s]\n",
           BOOL[MULGFLG1]);
   fprintf(stderr,
           "       -M M  : order of mel-generalized cepstrum (output)           [%d]\n",
           ORDER2);
   fprintf(stderr,
           "       -A A  : alpha of mel-generalized cepstrum (output)           [%g]\n",
           ALPHA2);
   fprintf(stderr,
           "       -G G  : gamma of mel-generalized cepstrum (output)           [%g]\n",
           GAMMA2);
   fprintf(stderr,
           "       -C C  : gamma of mel-generalized cepstrum = -1 / (int) C (output)\n");
   fprintf(stderr,
           "       -N    : regard output as normalized mel-generalized cepstrum [%s]\n",
           BOOL[NORMFLG2]);
   fprintf(stderr,
           "       -U    : regard output as multiplied by gamma                 [%s]\n",
           BOOL[MULGFLG2]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       mel-generalized cepstrum (%s)                   [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       transformed mel-generalized cepstrum (%s)\n",
           FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "      value of c and C must be c>=1, C>=1 \n");
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
   int m1 = ORDER1, m2 = ORDER2, i;
   double a1 = ALPHA1, a2 = ALPHA2, g1 = GAMMA1, g2 = GAMMA2, *c1, *c2;
   Boolean norm1 = NORMFLG1, norm2 = NORMFLG2, mulg1 = MULGFLG1, mulg2 =
       MULGFLG2;
   FILE *fp = stdin;

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
         case 'g':
            g1 = atof(*++argv);
            --argc;
            break;
         case 'c':
            g1 = atoi(*++argv);
            --argc;
            if (g1 < 1)
               fprintf(stderr, "%s : value of c must be c>=1!\n", cmnd);
            g1 = -1.0 / g1;
            break;
         case 'G':
            g2 = atof(*++argv);
            --argc;
            break;
         case 'C':
            g2 = atoi(*++argv);
            --argc;
            if (g2 < 1)
               fprintf(stderr, "%s : value of C must be C>=1!\n", cmnd);
            g2 = -1.0 / g2;
            break;
         case 'n':
            norm1 = 1 - norm1;
            break;
         case 'N':
            norm2 = 1 - norm2;
            break;
         case 'u':
            mulg1 = 1 - mulg1;
            break;
         case 'U':
            mulg2 = 1 - mulg2;
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

   if (mulg1 && g1 == 0) {
      fprintf(stderr,
              "%s : gamma for input mgc coefficients should not equal to 0 if you specify -u option!\n",
              cmnd);
      usage(1);
   }

   while (freadf(c1, sizeof(*c1), m1 + 1, fp) == m1 + 1) {

      if (norm1)
         ignorm(c1, c1, m1, g1);
      else if (mulg1)
         c1[0] = (c1[0] - 1.0) / g1;

      if (mulg1)
         for (i = m1; i >= 1; i--)
            c1[i] /= g1;

      mgc2mgc(c1, m1, a1, g1, c2, m2, a2, g2);

      if (norm2)
         gnorm(c2, c2, m2, g2);
      else if (mulg2)
         c1[0] = c1[0] * g2 + 1.0;

      if (mulg2)
         for (i = m2; i >= 1; i--)
            c2[i] *= g2;

      fwritef(c2, sizeof(*c2), m2 + 1, stdout);
   }

   return (0);
}
