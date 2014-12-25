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

/****************************************************************
* NAME :         fig0 - subroutines for fig                     *
****************************************************************/
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fig.h"
#include "plot.h"

#define xt(x) ((is_xlog<0) ? log10(x) : (x))
#define yt(y) ((is_ylog<0) ? log10(y) : (y))
#define abs(x) (((x)<0) ? -(x) : (x))
#define nz(mx, mn) ((mx>mn) ? mx : mn)


extern int ltype, type, is_t;
extern double xo, yo, xl, yl, x00, y00, mh, mw, h, w;
extern double xclip0, yclip0, xclip1, yclip1, wf, hf, fct;

static int lmod[] = { 0, 2, 6, 3, 4 }, ptyp = 1;

static int is_xlog = 0, is_ylog = 0;
static double lpit[] = { 10.0, 1.6, 10.0, 3.0, 5.0 };

static double dhat = 1.0, that = 45.0;
static double xfct = 1.0, yfct = 1.0;
static char label[BUFLNG / 2] = "";
static double xbuf[BUFLNG], ybuf[BUFLNG];

void graph(FILE * fp)
{
   static char buf[BUFLNG], arg[BUFLNG / 2], xtype[16], ytype[16];
   static double xa, ya, xap, yap, xmin, xmax, ymin, ymax;
   static double xs = -NSCALE, ys = -NSCALE;
   int n, c;
   char *s, *p;
   double x, y, lpt, th, dt, lscale, rad;
   int is_grid, old_lbl = 0;
   char xory;

   h *= fct;
   w *= fct;

   for (n = 0; (s = fgets(buf, BUFLNG, fp));) {
      s = getarg(s, arg);
      if (s == NULL || *arg == '#');    /* comment line */
      else if ((!is_t && strcmp(arg, "x") == 0)
               || (is_t && strcmp(arg, "y") == 0)) {
         s = gettyp(s, xtype);
         if (sscanf(s, "%lf %lf %lf", &xmin, &xmax, &xa) != 3)
            xa = xmin;
         if (strncmp(xtype, "log", 3) == 0) {
            xmin = log10(xmin);
            xmax = log10(xmax);
            xa = log10(xa);
            is_xlog = (xtype[3] == '*') ? -1 : 1;
         }
         xfct = xl / (xmax - xmin);
         xap = (xa - xmin) * xfct;
         x00 = -xmin * xfct;
      } else if ((!is_t && strcmp(arg, "y") == 0)
                 || (is_t && strcmp(arg, "x") == 0)) {
         s = gettyp(s, ytype);
         if (sscanf(s, "%lf %lf %lf", &ymin, &ymax, &ya) != 3)
            ya = ymin;
         if (strncmp(ytype, "log", 3) == 0) {
            ymin = log10(ymin);
            ymax = log10(ymax);
            ya = log10(ya);
            is_ylog = (ytype[3] == '*') ? -1 : 1;
         }
         yfct = (yl) ? yl / (ymax - ymin) : 0;
         yap = (ya - ymin) * yfct;
         y00 = -ymin * yfct;
      } else if ((!is_t && strncmp(arg, "xscale", 6) == 0)
                 || (is_t && strncmp(arg, "yscale", 6) == 0)) {
         is_grid = *(arg + 6);
         if (type < 0 || (ya != ymin && ya != ymax)) {
            plot(0.0, yap, 3);
            plot(xl, yap, 2);
         }
         ys = yap - h - MSCALE;
         while ((s = getarg(s, p = arg)) != NULL) {
            if (*p != '"') {
               x = atof((is_number(*p)) ? p : p + 1);
               if (strncmp(xtype, "mel", 3) == 0)
                  x = argapf(x / nz(xmax, xmin),
                             atof(xtype + 3)) * nz(xmax, xmin);
               else if (is_xlog)
                  x = log10(x);
               x = (x - xmin) * xfct;
               lscale = (*p == 's') ? LSCALE / 2 : LSCALE;
               if (*p != '\\' && *p != '@') {
                  plot(x, yap, 3);
                  plot(x, yap + lscale, 2);
                  if (type > 0 && !is_grid && yap == 0) {
                     plot(x, yl, 3);
                     plot(x, yl - lscale, 2);
                  }
               } else if (*p == '\\')
                  ++p;
            }
            if (is_number(*p) || *p++ == '"')
               _symbol(x - sleng(p, h, w) / 2, ys - ysadj(), p, h, w, 0.0);
         }
      } else if ((!is_t && strncmp(arg, "yscale", 6) == 0)
                 || (is_t && strncmp(arg, "xscale", 6) == 0)) {
         is_grid = *(arg + 6);
         if (type < 0 || (xa != xmin && xa != xmax)) {
            plot(xap, 0.0, 3);
            plot(xap, yl, 2);
         }
         while ((s = getarg(s, p = arg)) != NULL) {
            if (*p != '"') {
               y = atof((is_number(*p)) ? p : p + 1);
               if (strncmp(ytype, "mel", 3) == 0)
                  y = argapf(y / nz(ymax, ymin),
                             atof(ytype + 3)) * nz(ymax, ymin);
               else if (is_ylog)
                  y = log10(y);
               y = (y - ymin) * yfct;
               lscale = (*p == 's') ? LSCALE / 2 : LSCALE;
               if (*p != '\\' && *p != '@') {
                  plot(xap, y, 3);
                  plot(xap + lscale, y, 2);
                  if (type > 0 && !is_grid && xap == 0) {
                     plot(xl, y, 3);
                     plot(xl - lscale, y, 2);
                  }
               } else if (*p == '\\')
                  ++p;
            }
            if (is_number(*p) || *p++ == '"') {
               x = xap - sleng(p, h, w) - MSCALE;
               if (x < xs)
                  xs = x;
               _symbol(x, y - h * 0.5, p, h, w, 0.0);
            }
         }
      } else if (strcmp(arg + 1, "grid") == 0) {
         draw_fig0(xbuf, ybuf, n, wf, hf, fct);
         if ((!is_t && (*arg == 'x')) || (is_t && (*arg == 'y'))) {
            ybuf[0] = 0;
            ybuf[1] = yl;
            while ((s = getarg(s, arg)) != NULL) {
               x = atof(arg);
               if (is_xlog)
                  x = log10(x);
               xbuf[0] = xbuf[1]
                   = (x - xmin) * xfct;
               draw_fig0(xbuf, ybuf, 2, wf, hf, fct);
            }
         } else {
            xbuf[0] = 0;
            xbuf[1] = xl;
            while ((s = getarg(s, arg)) != NULL) {
               y = atof(arg);
               if (is_ylog)
                  y = log10(y);
               ybuf[0] = ybuf[1]
                   = (y - ymin) * yfct;
               draw_fig0(xbuf, ybuf, 2, wf, hf, fct);
            }
         }
         n = 0;
      } else if (strcmp(arg + 1, "circle") == 0) {
         xory = *arg;
         s = getarg(s, arg);
         x = xt(atof(arg));
         s = getarg(s, arg);
         y = yt(atof(arg));
         swap(&x, &y);
         x = xfct * x + x00;
         y = yfct * y + y00;
         while ((s = getarg(s, arg)) != NULL) {
            if ((!is_t && xory == 'x') || (is_t && xory == 'y'))
               rad = xt(atof(arg)) * xfct;
            else
               rad = yt(atof(arg)) * yfct;
            pntstyl(ptyp);
            circle(x, y, rad, rad, 0., 360.);
         }
      } else if (strcmp(arg, "circle") == 0) {
         s = getarg(s, arg);
         x = xt(atof(arg));
         s = getarg(s, arg);
         y = yt(atof(arg));
         swap(&x, &y);
         x = xfct * x + x00;
         y = yfct * y + y00;
         while ((s = getarg(s, arg)) != NULL) {
            rad = atof(arg);
            pntstyl(ptyp);
            circle(x, y, rad, rad, 0., 360.);
         }
      } else if (strcmp(arg + 1, "name") == 0) {
         s = getname(s, p = arg + 1);
         if ((!is_t && *arg == 'x') || (is_t && *arg == 'y'))
            _symbol((xl - sleng(s, h, w)) / 2,
                    (*p) ? -atof(p) - h : ys - h - NSCALE, s, h, w, 0.0);
         else
            _symbol((*p) ? -atof(p) : xs - MSCALE,
                    (yl - sleng(s, h, w)) / 2, s, h, w, 90.0);
      } else if (strncmp(arg, "title", 5) == 0 || strncmp(arg, "print", 5) == 0) {
         sscanf(s, "%lf %lf", &x, &y);
         swap(&x, &y);
         if (*arg == 'p') {
            x = xfct * xt(x) + x00;
            y = yfct * yt(y) + y00;
         }
         s = gettxt_fig(s);
         th = getarg(s + strlen(s) + 1, arg) ? atof(arg) : 0;
         if (*(arg + 5)) {
            x -= rx(LADJ * h / 2, h / 2, th);
            y -= ry(LADJ * h / 2, h / 2, th);
         }
         _symbol(x, y, s, h, w, th);
      } else if (strcmp(arg, "eod") == 0 || strcmp(arg, "EOD") == 0) {
         draw_fig0(xbuf, ybuf, n, wf, hf, fct);
         n = 0;
      } else if (strcmp(arg, "pen") == 0) {
         n = flush(xbuf, ybuf, n, wf, hf, fct);
         pen(atoi(s));
      } else if (strcmp(arg, "join") == 0) {
         n = flush(xbuf, ybuf, n, wf, hf, fct);
         join(atoi(s));
      } else if (strcmp(arg, "csize") == 0) {
         if (sscanf(s, "%lf %lf", &h, &w) != 2)
            w = h;
      } else if (strcmp(arg, "hight") == 0) {
         if (sscanf(s, "%lf %lf", &mh, &mw) != 2)
            mw = mh;
      } else if (strcmp(arg, "line") == 0) {
         n = flush(xbuf, ybuf, n, wf, hf, fct);
         if (sscanf(s, "%d %lf", &ltype, &lpt) != 2) {
            if (ltype > 0)
               lpt = lpit[ltype - 1];
         }
         if (--ltype >= 0)
            mode(lmod[ltype], lpt);
      } else if (strcmp(arg, "italic") == 0)
         italic(atof(s));
      else if (strcmp(arg, "mark") == 0) {
         while (*s == ' ' || *s == '\t')
            ++s;
         if (*s == '\\' && *(s + 1) == '0')
            *label = '\0';
         else
            strcpy(label, s);
      } else if (strcmp(arg, "paint") == 0) {
         sscanf(s, "%d %lf %lf", &ptyp, &dhat, &that);
      } else if (strcmp(arg, "clip") == 0) {
         draw_fig0(xbuf, ybuf, n, wf, hf, fct);
         for (n = 0; (s = getarg(s, arg)) != NULL; ++n) {
            x = xt(atof(arg));
            if ((s = getarg(s, arg)) == NULL)
               break;
            y = yt(atof(arg));
            swap(&x, &y);
            xbuf[n] = xfct * x + x00;
            ybuf[n] = yfct * y + y00;
         }
         if (n == 0) {
            xclip0 = yclip0 = 0;
            xclip1 = xl;
            yclip1 = yl;
            swap(&xclip1, &yclip1);
         } else if (n == 2) {
            xclip0 = xbuf[0];
            yclip0 = ybuf[0];
            xclip1 = xbuf[1];
            yclip1 = ybuf[1];
         }
         n = 0;
      } else if (strcmp(arg, "box") == 0) {
         draw_fig0(xbuf, ybuf, n, wf, hf, fct);
         for (n = 0; (s = getarg(s, arg)) != NULL; ++n) {
            x = xt(atof(arg));
            if ((s = getarg(s, arg)) == NULL)
               break;
            y = yt(atof(arg));
            swap(&x, &y);
            xbuf[n] = xfct * x + x00;
            ybuf[n] = yfct * y + y00;
         }
         if (n == 2) {
            xbuf[2] = xbuf[1];
            ybuf[3] = ybuf[2] = ybuf[1];
            ybuf[1] = ybuf[0];
            xbuf[3] = xbuf[0];
            n = 4;
         }
         polyg(xbuf, ybuf, n, wf, hf, fct);
         n = 0;
      } else {
         x = xt(atof(arg));
         s = getarg(s, arg);
         y = yt(atof(arg));
         swap(&x, &y);
         xbuf[n] = x = xfct * x + x00;
         ybuf[n] = y = yfct * y + y00;
         if (is_in(x, y) && ((s = getarg(s, arg))
                             || *label || old_lbl > 0)) {
            c = 0;
            if (s || *label) {
               if (s == NULL)
                  s = getarg(label, arg);
               if (*arg == '\\' && (abs(c = atoi(arg + 1))) < 16)
                  mark(abs(c), &x, &y, 1, mh, 1);
               else if (abs(c) == 16) {
                  pntstyl(ptyp);
                  circle(x, y, mh / 2, mh / 2, 0., 360.);
               } else {
                  if (c) {
                     *arg = c;
                     *(arg + 1) = '\0';
                  }
                  _symbol(x - LADJ * h / 2, y - w / 2, arg, h, w, atof(s));
               }
            }
            if (c > 0)
               n = flush(xbuf, ybuf, n, wf, hf, fct);
            if ((c > 0 || old_lbl > 0) && n) {
               dt = atan2(y - ybuf[0], x - xbuf[0]);
               if (old_lbl > 0) {
                  xbuf[0] += MADJ * mh * cos(dt);
                  ybuf[0] += MADJ * mh * sin(dt);
               }
               if (c > 0) {
                  xbuf[1] -= MADJ * mh * cos(dt);
                  ybuf[1] -= MADJ * mh * sin(dt);
               }
               draw_fig0(xbuf, ybuf, 2, wf, hf, fct);
               xbuf[0] = x;
               ybuf[0] = y;
               n = 0;
            }
            old_lbl = c;
         }
         if (++n >= BUFLNG)
            n = flush(xbuf, ybuf, n, wf, hf, fct);
      }
   }
   draw_fig0(xbuf, ybuf, n, wf, hf, fct);
}

void draw_fig0(double x[], double y[], int n, double w, double h, double fct)
{
   if (n && ltype >= 0) {
      bound(xclip0, yclip0, xclip1, yclip1);
      line(1, x, y, n);
      rstbnd(w, h, fct);
   }
}

int flush(double x[], double y[], int n, double w, double h, double fct)
{
   if (n > 1) {
      draw_fig0(x, y, n, w, h, fct);
      x[0] = x[n - 1];
      y[0] = y[n - 1];
      return (1);
   } else
      return (n);
}

void polyg(double x[], double y[], int n, double w, double h, double fct)
{
   bound(xclip0, yclip0, xclip1, yclip1);
   hatch(ptyp, x, y, n, dhat, that);
   rstbnd(w, h, fct);
}

int is_in(double x, double y)
{
   if (x >= 0 && x <= xl && y >= 0 && y <= yl)
      return (1);
   else
      return (0);
}

void swap(double *x, double *y)
{
   double t;

   if (is_t) {
      t = *y;
      *y = *x;
      *x = t;
   }
}
