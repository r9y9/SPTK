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
*    Draw a Graph                                                       *
*                                                                       *
*                                             1991.1  T.Kobayshi        *  
*                                             1996.5  K.Koishida        *
*                                                                       *
*       usage:                                                          *
*          fdrw [options] [infile] > stdout                             *
*       options:                                                        *
*          -F F     :  factor                              [1]          *
*          -R R     :  rotation angle                      [0]          *
*          -W W     :  width of figure                     [1]          *
*          -H H     :  height of figure                    [1]          *
*          -o xo yo :  origin in mm                        [20 25]      *
*          -g g     :  draw grid                           [1]          *
*          -m m     :  line type                           [0]          *
*          -l l     :  line pitch                          [0]          *
*          -p p     :  pen number                          [1]          *
*          -j j     :  join number                         [1]          *
*          -n n     :  number of sample                    [0]          *
*          -t t     :  coordinate type                     [0]          *
*          -y y1 y2 :  scaling factor                      [-1 1]       *
*          -z z     :  offset for duplication              [0]          *
*          -b       :  bar graph mode                      [FALSE]      *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: fdrw.c,v 1.29 2013/12/16 09:01:56 mataki Exp $";


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

#include "plot.h"

/*  Required Functions  */
int drw(FILE * fp);

/*  Default Values  */
#define BAR  FA
#define FCT  1.0
#define TH  0.0
#define XO  20.0
#define YO  25.0
#define GTYPE  1
#define LTYPE  0
#define LPT  0.0
#define PENNO  1
#define JOINNO 1
#define NSMPLS  0
#define DZ  0.0
#define WF  1.0
#define HF  1.0
#define XSIZE  100.0
#define YSIZE  100.0
#define CTYPE  0
#define YMIN  -1.0
#define YMAX  1.0
#define BUF_LNG  5000
#define H  2.5
#define SC  1.5

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - draw a graph\n", cmnd);
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
   fprintf(stderr, "       -g g     : draw grid              [%d]\n", GTYPE);
   fprintf(stderr, "       -m m     : line type              [%d]\n", LTYPE);
   fprintf(stderr, "       -l l     : line pitch             [%g]\n", LPT);
   fprintf(stderr, "       -p p     : pen number             [%d]\n", PENNO);
   fprintf(stderr, "       -j j     : join number            [%d]\n", JOINNO);
   fprintf(stderr, "       -n n     : number of sample       [%d]\n", NSMPLS);
   fprintf(stderr, "       -t t     : coordinate type        [%d]\n", CTYPE);
   fprintf(stderr, "       -y y1 y2 : scaling factor         [%g %g]\n", YMIN,
           YMAX);
   fprintf(stderr, "       -z z     : offset for duplication [%g]\n", DZ);
   fprintf(stderr, "       -b       : bar graph mode         [%s]\n",
           BOOL[BAR]);
   fprintf(stderr, "       -h       : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)\n", FORMAT);
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

double fct = FCT, th = TH, lpt = LPT, dz = DZ,
    scale = 0, xo = XO, yo = YO, xl = XSIZE, yl = YSIZE,
    ymin = YMIN, ymax = YMAX, wf = WF, hf = HF;
long nsmpls = NSMPLS;
int is_hold = BAR, is_y = 0;
int ltype = LTYPE, gtype = GTYPE, penno = PENNO, joinno = JOINNO, ctype = CTYPE;
int lmod[] = { 0, 2, 6, 3, 4 };
double lpit[] = { 10, 1.6, 10, 3, 5 };


int main(int argc, char *argv[])
{
   FILE *fp = stdin;
   char *s;
   int c;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if (c != 'b' && *++s == '\0') {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'b':
            is_hold = 1;
            break;
         case 'F':
            fct = atof(s);
            break;
         case 'R':
            th = atof(s);
            break;
         case 'o':
            xo = atof(s);
            yo = atof(*++argv);
            --argc;
            break;
         case 'g':
            gtype = atoi(s);
            break;
         case 'm':
            ltype = atoi(s) - 1;
            break;
         case 'l':
            lpt = atof(s);
            break;
         case 'p':
            penno = atoi(s);
            break;
         case 'j':
            joinno = atoi(s);
            break;
         case 'n':
            nsmpls = atoi(s);
            break;
         case 'z':
            dz = atof(s);
            break;
         case 'W':
            wf = atof(s);
            xl = atof(s) * XSIZE;
            break;
         case 'H':
            hf = atof(s);
            yl = atof(s) * YSIZE;
            break;
         case 't':
            ctype = (atoi(s) < 0) ? -1 : 1;
            break;
         case 'y':
            is_y = 1;
            ymin = atof(s);
            ymax = atof(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            usage(1);
            break;
         }
      } else
         fp = getfp(*argv, "rb");
   }
   plots(0x81);
   mode(lmod[ltype], (lpt == 0) ? lpit[ltype] : lpt);
   plot(xo, yo, -3);
   rotate(th);
   pen(1);
   join(0);
   xl *= fct;
   yl *= fct;
   if (gtype) {
      if (ctype) {
         plot(-ctype * yl, 0.0, 2);
         plot(-ctype * yl, ctype * xl, 2);
         plot(0.0, ctype * xl, 2);
         plot(0.0, 0.0, 2);
      } else {
         plot(xl, 0.0, 2);
         plot(xl, yl, 2);
         plot(0.0, yl, 2);
         plot(0.0, 0.0, 2);
      }
   }
   pen(penno);
   join(joinno);
   while (!feof(fp)) {
      drw(fp);
      if (dz != 0.0) {
         if (ctype)
            plot(-ctype * dz, 0.0, -3);
         else
            plot(0.0, dz, -3);
      }
   }
   plote();
   return (0);
}

int drw(FILE * fp)
{
   int k, nitems;
   char buf[64];
   int n, n2;
   double dx = 0.0, h;
   double *p;
   double *x, *y;

   if ((x = calloc(BUF_LNG * 4, sizeof(*x))) == NULL)
      return (1);
   y = x + BUF_LNG * 2;

   if (nsmpls)
      dx = xl / (is_hold ? nsmpls : nsmpls - 1);
   for (n = 0; n == 0 || (n < nsmpls && !feof(fp));) {
      if (nsmpls)
         nitems = (nsmpls - n > BUF_LNG) ? BUF_LNG : nsmpls - n;
      else
         nitems = BUF_LNG;
      if ((nitems = freadf(y, sizeof(*y), nitems, fp)) == 0)
         break;
      if (nsmpls == 0) {
         nsmpls = nitems;
         dx = xl / (is_hold ? nitems : nitems - 1);
      }
      if (scale == 0) {
         if (!is_y)
            for (ymin = ymax = y[0], k = 1; k < nitems; ++k) {
               if (y[k] < ymin)
                  ymin = y[k];
               else if (y[k] > ymax)
                  ymax = y[k];
            }
         if (ymax == ymin) {
            ymax += 1;
            ymin -= 1;
         }
         scale = yl / (ymax - ymin);
         if (gtype) {
            h = H * ((xl > XSIZE) ? XSIZE : xl) / XSIZE;
            sprintf(buf, "0 -x- %ld  %g -y- %g", nsmpls - 1, ymin, ymax);

            h *= fct;

            if (ctype == 0)
               symbol(0.0, -h - 2, buf, h, h, 0.0);
            else if (ctype > 0)
               symbol(h + 2, 0.0, buf, h, h, 90.0);
            else
               symbol(0.0, h + 2, buf, h, h, 0.0);
         }
         if (ctype) {
            if (ctype > 0)
               bound(-yl, 0.0, 0.0, xl);
            else
               bound(0.0, -xl, yl, 0.0);
            plot(ctype * scale * ymin, 0.0, -3);
         } else {
            bound(0.0, 0.0, xl, yl);
            plot(0.0, -scale * ymin, -3);
         }
         if (gtype && ymin < 0 && ymax > 0) {
            plot(0.0, 0.0, 3);
            if (gtype > 1) {
               if (ctype)
                  plot(0.0, ctype * xl, 2);
               else
                  plot(xl, 0.0, 2);
            } else {
               if (ctype)
                  plot(0.0, ctype * SC * fct, 2);
               else
                  plot(SC * fct, 0.0, 2);
            }
         }
      }
      for (k = 0; k < nitems; ++k) {
         x[k] = n++ * dx;
         y[k] *= scale;
      }
      if (is_hold) {
         for (k = nitems, p = y + nitems * 2; --k >= 0;) {
            *--p = y[k];
            *--p = y[k];
         }
         p = x + nitems * 2;
         *--p = n * dx;
         for (k = nitems; --k > 0;) {
            *--p = x[k];
            *--p = x[k];
         }
         n2 = 2 * nitems;
      } else
         n2 = nitems;

      if (ctype) {
         for (k = 0; k < n2; ++k) {
            x[k] *= ctype;
            y[k] *= -ctype;
         }
         if (n != nitems)
            plot(y[0], x[0], 2);
         line(1, y, x, n2);
      } else {
         if (n != nitems)
            plot(x[0], y[0], 2);
         line(1, x, y, n2);
      }
   }
   rstbnd(wf, hf, fct);
   return (0);
}
