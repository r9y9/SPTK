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
*    Extract Frame from Data Sequence                                   *
*                                                                       *
*                                       1985.11 K.Tokuda                *
*                                       1996.4  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               frame [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -l l     :  frame length                [256]           *
*               -p p     :  frame period                [100]           *
*               -n       :  no center start point       [FALSE]         *
*       infile:                                                         *
*               data sequence                                           *
*                   , x(0), x(1), ...,                                  *
*       stdout:                                                         *
*               frame sequence                                          *
*                   0, 0, ..., 0, x(0), x(1), ..., x(l/2),              *
*                   , x(t), x(t+1),       ...,       x(t+l-1),          *
*                   , x(2t), x(2t+1),     ....                          *
*              if -n specified                                          *
*                   x(0), x(1),           ...,       x(l),              *
*                   , x(t), x(t+1),       ...,       x(t+l-1),          *
*                   , x(2t), x(2t+1),     ....                          *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: frame.c,v 1.27 2014/12/11 08:30:35 uratec Exp $";


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
#define LENG 256
#define FPERIOD 100
#define NOCTR FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - extract frame from data sequence\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : frame length          [%d]\n", LENG);
   fprintf(stderr, "       -p p  : frame period          [%d]\n", FPERIOD);
   fprintf(stderr, "       -n    : no center start point [%s]\n", BOOL[NOCTR]);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence                 [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       extracted data sequence\n");
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
   int l = LENG, fprd = FPERIOD, ns, i, rnum, ts, cs;
   FILE *fp = stdin;
   Boolean noctr = NOCTR;
   double *x, *xx, *p1, *p2, *p;
   char *s, c;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 'l':
            l = atoi(*++argv);
            --argc;
            break;
         case 'p':
            fprd = atoi(*++argv);
            --argc;
            break;
         case 'n':
            noctr = 1 - noctr;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   x = dgetmem(l);
   if (!noctr) {
      i = (int) ((l + 1) / 2);
      rnum = freadf(&x[(int) (l / 2)], sizeof(*x), i, fp);
   } else
      rnum = freadf(x, sizeof(*x), l, fp);
   if (rnum == 0)
      return 0;
   cs = rnum;
   fwritef(x, sizeof(*x), l, stdout);

   if ((ns = (l - fprd)) > 0) {
      p = &x[fprd];
      for (;;) {
         p1 = x;
         p2 = p;
         i = ns;
         while (i--) {
            *p1++ = *p2++;
         }
         rnum = freadf(p1, sizeof(*p1), fprd, fp);
         if (rnum < fprd) {
            ts = fprd - rnum;
            cs -= ts;
            while (rnum--)
               p1++;
            while (ts--)
               *p1++ = 0.0;
         }
         if (cs <= 0)
            break;
         fwritef(x, sizeof(*x), l, stdout);
      }
   } else {
      i = -ns;
      xx = dgetmem(i);
      for (;;) {
         if (freadf(xx, sizeof(*xx), i, fp) != i)
            break;
         rnum = freadf(x, sizeof(*x), l, fp);
         if (rnum < l) {
            if (rnum == 0)
               break;
            ts = l - rnum;
            p1 = x;
            while (rnum--)
               p1++;
            while (ts--)
               *p1++ = 0.0;
         }
         fwritef(x, sizeof(*x), l, stdout);
      }
   }

   return 0;
}
