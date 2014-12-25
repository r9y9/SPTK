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
*    Transform LPC to PARCOR                                            *
*                                                                       *
*                                        1988    K.Tokuda               *
*                                        1996.1  K.Koishida             *
*                                                                       *
*       usage:                                                          *
*               lpc2par [ options ] [ infile ] > stdout                 *
*       options:                                                        *
*               -m m     :  order of LPC                     [25]       *
*               -g g     :  gamma of generalized cepsrtum    [1]        *
*               -c  c    :  gamma of generalized cepstrum (input)       *
*                           gamma = -1 / (int) c                        *
*               -s       :  check stable or unstable         [FALSE]    *
*       infile:                                                         *
*               LP Coefficeints                                         *
*                       , K, a(1), ..., a(m),                           *
*       stdout:                                                         *
*               PARCOR                                                  *
*                       , K, k(1), ..., k(m),                           *
*       require:                                                        *
*               lpc2par()                                               *
*       notice:                                                         *
*               value of c must be c>=1                                 *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: lpc2par.c,v 1.27 2014/12/11 08:30:40 uratec Exp $";


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
#define ORDER 25
#define GAMMA 1.0
#define STABLE FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transform LPC to PARCOR\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of LPC                  [%d]\n",
           ORDER);
   fprintf(stderr, "       -g g  : gamma of generalized cepstrum [%g]\n",
           GAMMA);
   fprintf(stderr,
           "       -c c  : gamma of mel-generalized cepstrum = -1 / (int) c (input) \n");
   fprintf(stderr, "       -s    : check stable or unstable      [%s]\n",
           BOOL[STABLE]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  stdin\n");
   fprintf(stderr, "       LP coefficients (%s)               [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       PARCOR (%s) or \n", FORMAT);
   fprintf(stderr,
           "       0 <stable>, -1 <unstable> (int) if -s option is specified\n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       value of c must be c>=1\n");
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
   int m = ORDER, stable, i;
   FILE *fp = stdin;
   double *k, *a, g = GAMMA;
   Boolean flags = STABLE;

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
         case 'g':
            g = atof(*++argv);
            --argc;
            break;
         case 'c':
            g = atoi(*++argv);
            --argc;
            if (g < 1)
               fprintf(stderr, "%s : value of c must be c>=1!\n", cmnd);
            g = -1.0 / g;
            break;
         case 's':
            flags = 1 - flags;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   a = dgetmem(m + m + 2);
   k = a + m + 1;

   while (freadf(a, sizeof(*a), m + 1, fp) == m + 1) {
      if (g != 1.0)
         for (i = 1; i <= m; i++)
            a[i] *= g;

      stable = lpc2par(a, k, m);

      if (flags)
         fwritex(&stable, sizeof(stable), 1, stdout);
      else
         fwritef(k, sizeof(*k), m + 1, stdout);
   }

   return (0);
}
