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
*    Generate Normal Distrubuted Random Value                           *
*                                                                       *
*                                        1991.9 T.Kanno                 *
*                                        1996.1 N.Miyazaki modified     *
*                                                                       *
*       usage:                                                          *
*               nrand [ options ] > stdout                              *
*       options:                                                        *
*               -l l  : output length      [256]                        *
*               -s s  : seed for nrand     [1]                          *
*               -m m  : mean               [0.0]                        *
*               -v v  : variance           [1.0]                        *
*               -d d  : standard deviation [1.0]                        *
*       stdout:                                                         *
*               random value (float)                                    *
*       notice:                                                         *
*               if l<0, generate infinite sequence                      *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: nrand.c,v 1.23 2013/12/16 09:02:02 mataki Exp $";


/*  Standard C Libraries  */
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

/*  Default Values  */
#define LENG 256
#define SEED 1
#define MEAN 0.0
#define VAR  1.0
#define SDEV 1.0

/*  Command Name  */
char *cmnd;


void usage(void)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - generate normal distributed random value\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : output length      [%d]\n", LENG);
   fprintf(stderr, "       -s s  : seed for nrand     [%d]\n", SEED);
   fprintf(stderr, "       -m m  : mean               [%g]\n", MEAN);
   fprintf(stderr, "       -v v  : variance           [%g]\n", VAR);
   fprintf(stderr, "       -d d  : standard deviation [%g]\n", SDEV);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       random values (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       if l<0, generate infinite sequence\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(1);
}


int main(int argc, char *argv[])
{
   char *str, flg;
   int leng = LENG, seed = SEED, i;
   unsigned long next = SEED;
   double p, mean = MEAN, sdev = SDEV;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(str = *++argv) == '-') {
         flg = *++str;
         if (*++str == '\0') {
            str = *++argv;
            argc--;
         }
         switch (flg) {
         case 'l':
            leng = atoi(str);
            break;
         case 's':
            seed = atoi(str);
            break;
         case 'm':
            mean = atof(str);
            break;
         case 'v':
            sdev = atof(str);
            sdev = sqrt(sdev);
            break;
         case 'd':
            sdev = atof(str);
            break;
         case 'h':
         default:
            usage();
         }
      } else
         usage();
   }

   if (seed != 1)
      next = srnd((unsigned int) seed);

   for (i = 0;; i++) {
      p = (double) nrandom(&next);
      p = mean + sdev * p;
      fwritef(&p, sizeof(p), 1, stdout);

      if (i == leng - 1)
         break;
   }

   return (0);
}
