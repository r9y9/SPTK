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
*    Generate excitation                                                *
*                                                                       *
*                                       1986.6  K.Tokuda                *
*                                       1996.4  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               excite [ options ] [ infile ] > stdout                  *
*       options:                                                        *
*               -p p     :  frame period                        [100]   *
*               -i i     :  interpolation period                [1]     *
*               -n       :  gauss/M-sequence flag for unoiced   [FALSE] *
*                           default is M-sequence                       *   
*               -s s     :  seed for nrand                      [1]     * 
*      infile:                                                          *
*               pitch data                                              *
*      stdout:                                                          *
*               excitation                                              *
*      require:                                                         *
*               mseq()                                                  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: excite.c,v 1.24 2013/12/16 09:01:55 mataki Exp $";


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
#define FPERIOD 100
#define IPERIOD 1
#define SEED 1
#define GAUSS FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - generate excitation\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -p p  : frame period                  [%d]\n",
           FPERIOD);
   fprintf(stderr, "       -i i  : interpolation period          [%d]\n",
           IPERIOD);
   fprintf(stderr, "       -n    : gauss/M-sequence for unvoiced [%s]\n",
           BOOL[GAUSS]);
   fprintf(stderr, "                   default is M-sequence\n");
   fprintf(stderr, "       -s s  : seed for nrand                [%d]\n", SEED);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       pitch period (%s)         [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       excitation (%s)\n", FORMAT);
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
   int fprd = FPERIOD, iprd = IPERIOD, i, j, seed = SEED;
   unsigned long next = SEED;
   FILE *fp = stdin;
   double x, p1, p2, inc, pc;
   Boolean gauss = GAUSS;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'p':
            fprd = atoi(*++argv);
            --argc;
            break;
         case 'i':
            iprd = atoi(*++argv);
            --argc;
            break;
         case 'n':
            gauss = TR;
            break;
         case 's':
            seed = atoi(*++argv);
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

   if (gauss & (seed != 1))
      next = srnd((unsigned int) seed);

   if (freadf(&p1, sizeof(p1), 1, fp) != 1)
      return (1);

   pc = p1;

   for (;;) {
      if (freadf(&p2, sizeof(p2), 1, fp) != 1)
         return (0);

      if ((p1 != 0.0) && (p2 != 0.0))
         inc = (p2 - p1) * (double) iprd / (double) fprd;
      else {
         inc = 0.0;
         pc = p2;
         p1 = 0.0;
      }

      for (j = fprd, i = (iprd + 1) / 2; j--;) {
         if (p1 == 0.0) {
            if (gauss)
               x = (double) nrandom(&next);
            else
               x = mseq();
         } else {
            if ((pc += 1.0) >= p1) {
               x = sqrt(p1);
               pc = pc - p1;
            } else
               x = 0.0;
         }

         fwritef(&x, sizeof(x), 1, stdout);

         if (!--i) {
            p1 += inc;
            i = iprd;
         }
      }
      p1 = p2;
   }

   return 0;
}
