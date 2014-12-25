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
*    Interpolation of data sequence                                     *
*                                                                       *
*                                            1996.4  K.Koishida         *
*                                                                       *
*       usage:                                                          *
*               interpolate [ options ] [ infile ] > stdout             *
*       options:                                                        *
*               -l l     :  length of vector       [1]                  *
*               -p p     :  interpolation period   [10]                 *
*               -s s     :  start sample           [0]                  *
*       infile:                                                         *
*               data sequence                                           *
*                        , x(0), x(1), ...                              *
*       stdout:                                                         *
*               decimated data                                          *
*                                                                       *
*       if -d is not specified (default)                                *
*                , 0, ..., 0, x(0), 0, ..., x(1), 0, ...,               *
*        --------- ---------- -------------                             *
*                      s-1          p       p                           *
*                                                                       *
*       otherwise,                                                      *
*                , 0, ..., 0, x(0), x(0), ..., x(1), x(1), ...,         *
*        --------- ---------- ----------------                          *
*                      s-1            p        p                        *
*                                                                       *
************************************************************************/

static char *rcs_id =
    "$Id: interpolate.c,v 1.28 2014/12/11 08:30:38 uratec Exp $";


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
#define PERIOD 10
#define START 0
#define PADINPUT FA
#define LENG 1

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - interpolation of data sequence\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : length of vector             [%d]\n", LENG);
   fprintf(stderr, "       -p p  : interpolation period         [%d]\n",
           PERIOD);
   fprintf(stderr, "       -s s  : start sample                 [%d]\n", START);
   fprintf(stderr, "       -d    : padding input rather than 0  [%s]\n",
           BOOL[PADINPUT]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)        [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       interpolated data sequence (%s)\n", FORMAT);
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
   int i, j = 0, period = PERIOD, start = START, fleng = LENG;
   FILE *fp = stdin;
   double *x;
   Boolean padinput = PADINPUT;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'p':
            period = atoi(*++argv);
            --argc;
            break;
         case 's':
            start = atoi(*++argv);
            --argc;
            break;
         case 'l':
            fleng = atoi(*++argv);
            --argc;
            break;
         case 'd':
            padinput = 1 - padinput;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   x = dgetmem(period * fleng);

   fwritef(x, sizeof(*x), start * fleng, stdout);

   while (freadf(x, sizeof(*x), fleng, fp) == fleng) {
      if (padinput) {
         for (i = fleng; i < period * fleng; i++) {
            if (j >= fleng)
               j = 0;
            x[i] = x[j];
            j++;
         }
      }

      fwritef(x, sizeof(*x), period * fleng, stdout);
   }

   return (0);
}
