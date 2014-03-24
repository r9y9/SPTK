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
*    Transform real sequence to phase                                   *
*                                                                       *
*                                      1989.6  K.Tokuda                 *
*                                      1996.2  N.Isshiki Modified       *
*                                                                       *
*       usage:                                                          *
*               phase [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -l l : frame length                     [256]           *
*               -p p : numerator coefficients file      [NULL]          *
*               -z z : denominator coefficients file    [NULL]          *
*               -m m : order of denominator polynomial  [L-1]           *
*               -n n : order of numerator polynomial    [L-1]           *
*               -u   : unlapping                        [TRUE]          *
*       infile:                                                         *
*               input sequense (float)                                  *
*       stdout:                                                         *
*               P0, P1, ..., PL/2 (float)(pi radian)                    *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: phase.c,v 1.25 2013/12/16 09:02:02 mataki Exp $";


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

/* Default Values */
#define FLMLNG 256
#define UNLAP  TR

char *BOOL[] = { "FALSE", "TRUE" };

/* Command Name */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - transform real sequence to phase\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : frame length                    [%d]\n",
           FLMLNG);
   fprintf(stderr, "       -p p  : numerator coefficients file     [NULL]\n");
   fprintf(stderr, "       -z z  : denominator coefficients file   [NULL]\n");
   fprintf(stderr, "       -m m  : order of denominator polynomial [L-1]\n");
   fprintf(stderr, "       -n n  : order of numerator polynomial   [L-1]\n");
   fprintf(stderr, "       -u    : unlapping                       [%s]\n",
           BOOL[UNLAP]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)                   [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       phase (%s) [pi rad]\n", FORMAT);
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
   int flng = FLMLNG, m = -1, n = -1;
   int unlap = UNLAP;
   double *p, *z, *ph;
   char *file_z = "", *file_p = "";
   FILE *fp_z = NULL, *fp_p = NULL;
   int no;

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
         case 'n':
            n = atoi(*++argv);
            --argc;
            break;
         case 'l':
            flng = atoi(*++argv);
            --argc;
            break;
         case 'z':
            file_z = *++argv;
            --argc;
            break;
         case 'p':
            file_p = *++argv;
            --argc;
            break;
         case 'u':
            unlap = 1 - unlap;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else {
         fprintf(stderr, "%s : Input file is not available!\n", cmnd);
         usage(1);
      }
   if (m < 0)
      m = flng - 1;
   if (n < 0)
      n = flng - 1;
   no = flng / 2 + 1;

   p = dgetmem(2 * flng + n);
   z = &p[flng];
   ph = &z[flng];

   if (*file_z != '\0')
      fp_z = getfp(file_z, "rb");

   if (*file_p != '\0')
      fp_p = getfp(file_p, "rb");

   if (*file_z == '\0' && *file_p == '\0')
      fp_z = stdin;
   for (;;) {
      if (*file_z == '\0' && *file_p != '\0') {
         z[0] = 1;
         n = 0;
      } else {
         if (freadf(z, sizeof(*z), n + 1, fp_z) == 0)
            return (0);
      }

      if (*file_p == '\0') {
         p[0] = 1;
         m = 0;
      } else {
         if (freadf(p, sizeof(*p), m + 1, fp_p) == 0)
            return (0);
      }
      phase(p, m, z, n, ph, flng, unlap);
      fwritef(ph, sizeof(*ph), no, stdout);
   }

   return (0);
}
