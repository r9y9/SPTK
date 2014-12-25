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
*    u-law PCM                                                          *
*                                                                       *
*                                      1991.3  T.Kanno                  *
*                                      1996.11 K.Koishida               *
*                                                                       *
*       usage:                                                          *
*               ulaw [ options ] [ infile ] > stdout                    *
*       options:                                                        *
*               -v v     :  maximum of input     [32768]                *
*               -u u     :  compression ratio    [256]                  *
*               -c       :  coder mode           [TRUE]                 *
*               -d       :  decoder mode         [FALSE]                *
*       infile:                                                         *
*               input sequence                                          *
*                       , x(0), x(1), ...,                              *
*       stdout:                                                         *
*               compressed sequence                                     *
*                       , x'(0), x'(1), ...,                            *
*       require:                                                        *
*               ulaw_e(), ulaw_d()                                      *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: ulaw.c,v 1.24 2014/12/11 08:30:51 uratec Exp $";


/*  Standard C Libraries  */
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
#define MAXVALUE 32768.0
#define MU       256.0
#define CODER    TR
#define DECODER  FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - u-law PCM\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -v v  : maximum of input    [%g]\n", MAXVALUE);
   fprintf(stderr, "       -u u  : compression ratio   [%g]\n", MU);
   fprintf(stderr, "       -c    : coder mode          [%s]\n", BOOL[CODER]);
   fprintf(stderr, "       -d    : decoder mode        [%s]\n", BOOL[DECODER]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       input sequence      (%s) [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       compressed sequence (%s)\n", FORMAT);
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
   double x, y, max = MAXVALUE, mu = MU, atof();
   Boolean decoder = DECODER;
   FILE *fp = stdin;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'v':
            max = atof(*++argv);
            --argc;
            break;
         case 'u':
            mu = atof(*++argv);
            --argc;
            break;
         case 'c':
            decoder = FA;
            break;
         case 'd':
            decoder = TR;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   if (!decoder)
      while (freadf(&x, sizeof(x), 1, fp) == 1) {
         y = ulaw_c(x, max, mu);
         fwritef(&y, sizeof(y), 1, stdout);
   } else
      while (freadf(&x, sizeof(x), 1, fp) == 1) {
         y = ulaw_d(x, max, mu);
         fwritef(&y, sizeof(y), 1, stdout);
      }

   return (0);
}
