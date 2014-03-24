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
*    Plot a Graph                                                       *
*                                                                       *
*                                            1990.9  T.Kobayashi        *
*                                            1996.5  K.Koishida         *
*                                                                       *
*       usage:                                                          *
*          fig [options] [infile] > stdout                              *
*       options:                                                        *
*          -F F     :  factor                      [1]                  *
*          -R R     :  rotation angle              [0]                  *
*          -W W     :  width of figure             [1]                  *
*          -H H     :  height of figure            [1]                  *
*          -o xo yo :  origin in mm                [20 20]              *
*          -g g     :  draw grid                   [2]                  *
*          -p p     :  pen number                  [1]                  *
*          -j j     :  join number                 [0]                  *
*          -s s     :  font size                   [1]                  *
*          -f f     :  first opened file           [NULL]               *
*          -t       :  transpose x and y axes      [FALSE]              *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: fig.c,v 1.27 2013/12/16 09:01:56 mataki Exp $";


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

#include "plot.h"


/*  Default Values  */
#define LENG 100
#define CH_NRML 2.5
#define CH_BIG 5.0
#define CW_NRML 2.2
#define CW_BIG 2.6
#define CW_HUGE 4.4

#define TYPE  2
#define FCT  1.0
#define TH  0.0
#define FNTSIZE  0
#define IS_T  FA
#define XO  20.0
#define YO  20.0
#define PENNO  1
#define JOINNO 0
#define XL  LENG
#define YL  LENG
#define WF  1.0
#define HF  1.0
char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - plot a graph\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -F F     : factor                 [%g]\n", FCT);
   fprintf(stderr, "       -R R     : rotation angle         [%g]\n", TH);
   fprintf(stderr, "       -W W     : width of figure        [1]\n");
   fprintf(stderr, "       -H H     : height of figure       [1]\n");
   fprintf(stderr, "       -o xo yo : origin in mm           [%g %g]\n", XO,
           YO);
   fprintf(stderr, "       -g g     : draw grid              [%d]\n", TYPE);
   fprintf(stderr, "       -p p     : pen number             [%d]\n", PENNO);
   fprintf(stderr, "       -j j     : join number            [%d]\n", JOINNO);
   fprintf(stderr, "       -s s     : font size              [%d]\n",
           FNTSIZE + 1);
   fprintf(stderr, "       -f f     : first opened file      [NULL]\n");
   fprintf(stderr, "       -t       : transpose x and y axes [%s]\n",
           BOOL[IS_T]);
   fprintf(stderr, "       -h       : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       command\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       plotter command\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

int ltype = 0, penno = PENNO, joinno = JOINNO, type = TYPE, is_t = IS_T;
double xo = XO, yo = YO;
double xl = XL, yl = YL, x00 = 0, y00 = 0;
double mh = 2, mw = 2, h = CH_NRML, w = CW_NRML;
double xclip0 = 0, yclip0 = 0, xclip1 = 0, yclip1 = 0;
double fct = FCT;
double wf = WF, hf = HF;

int main(int argc, char *argv[])
{
   FILE *fp;
   char *infile[16], *file = NULL;
   char *s;
   double th = TH;
   int c, n, nfiles, fntsize = FNTSIZE;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   for (nfiles = 0; --argc;) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if (*++s == '\0' && c != 't') {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'g':
            type = atoi(s);
            break;
         case 'F':
            fct = atof(s);
            break;
         case 'R':
            th = atof(s);
            break;
         case 's':
            fntsize = atoi(s) - 1;
            break;
         case 't':
            is_t = 1 - is_t;
            break;
         case 'o':
            xo = atof(s);
            yo = atof(*++argv);
            --argc;
            break;
         case 'f':
            file = s;
            break;
         case 'p':
            penno = atoi(s);
            break;
         case 'j':
            joinno = atoi(s);
            break;
         case 'W':
            wf = atof(s);
            xl = atof(s) * LENG;
            break;
         case 'H':
            hf = atof(s);
            yl = atof(s) * LENG;
            break;
         case 'h':
            usage(0);
         default:
            usage(1);
         }
      } else
         infile[nfiles++] = *argv;
   }

   if (fntsize < 0 || fntsize > 3)
      fntsize = 0;
   if (fntsize & 1)
      h = CH_BIG;
   if (fntsize & 2)
      w = CW_HUGE;
   if (fntsize == 1)
      w = CW_BIG;

   plots(0x81);
   org(xo, yo);
   rstbnd(wf, hf, fct);
   org(0.0, 0.0);
   font(fntsize);
   font(0);                     /* valid only LBP */
   plot(xo, yo, -3);
   rotate(th);
   pen(1);
   join(0);
   italic(0.0);
   mode(0, 10);
   xl *= fct;
   yl *= fct;
   xclip1 = xl;
   yclip1 = yl;
   swap(&xclip1, &yclip1);
   if (type--) {
      plot(0.0, 0.0, -3);
      plot(xl, 0.0, 2);
      plot(xl, yl, type ? 2 : 3);
      plot(0.0, yl, type ? 2 : 3);
      plot(0.0, 0.0, 2);
   }
   pen(penno);
   join(joinno);

   if (file) {
      fp = getfp(file, "rt");
      graph(fp);
      fclose(fp);
   }
   if (nfiles == 0)
      graph(stdin);
   else {
      for (n = 0; n < nfiles; ++n) {
         fp = getfp(infile[n], "rt");
         graph(fp);
         fclose(fp);
      }
   }
   plote();
   return (0);
}
