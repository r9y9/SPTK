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
*    data clipping                                                      *
*                                                                       *
*                                     1989.8.31   K.Tokuda              *
*                                                                       *
*       usage:                                                          *
*               clip [option] [infile] > stdout                         *
*       options:                                                        *
*               -y    ymin ymax : lower bound & upper bound [-1.0 1.0]  *
*               -ymin ymin      : lower bound (ymax = inf)  [N/A]       *
*               -ymax ymax      : upper bound (ymin = -inf) [N/A]       *
*       infile:                                                         *
*               data sequence (float)                                   *
*       stdout:                                                         *
*               clipped data sequence (float)                           *
*       require:                                                        *
*               clip()                                                  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: clip.c,v 1.23 2014/12/11 08:30:32 uratec Exp $";


/* Standard C Libraries */
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

/* Default Value */
#define FLT_MAX 3.4e+38
#define FLT_MIN -3.4e+38
#define YMIN -1.0
#define YMAX 1.0

/* Command Name */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - data clipping\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ option ] [ infile ] > outfile\n", cmnd);
   fprintf(stderr, "  option:\n");
   fprintf(stderr,
           "       -y    ymin ymax : lower bound & upper bound [-1.0 1.0]\n");
   fprintf(stderr,
           "       -ymin ymin      : lower bound (ymax = inf)  [N/A]\n");
   fprintf(stderr,
           "       -ymax ymax      : upper bound (ymin = -inf) [N/A]\n");
   fprintf(stderr, "       -h              : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)                       [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       clipped data sequence (%s)\n", FORMAT);
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
   FILE *fp = stdin;
   int size;
   double ymin = YMIN, ymax = YMAX;
   double *x, *y;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         argc--;
         switch (*(*argv + 1)) {
         case 'y':
            if ((*(*argv + 2)) == 'm') {
               switch (*(*argv + 3)) {
               case 'i':
                  ymin = atof(*++argv);
                  ymax = FLT_MAX;
                  break;
               case 'a':
                  ymax = atof(*++argv);
                  ymin = FLT_MIN;
                  break;
               }
            } else {
               ymin = atof(*++argv);
               argc--;
               ymax = atof(*++argv);
            }
            break;
         case 'h':
         default:
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   x = dgetmem(2 * 256);
   y = x + 256;

   while ((size = freadf(x, sizeof(*x), 256, fp)) != 0) {
      clip(x, size, ymin, ymax, y);
      fwritef(y, sizeof(*y), size, stdout);
   }

   return 0;
}
