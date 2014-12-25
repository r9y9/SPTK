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
* $Id: plot.c,v 1.25 2014/12/11 08:30:52 uratec Exp $         *
*          Interpret X-Y Ploter Commands                        *
****************************************************************/

#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "xgr.h"
#include "gcdata.h"


#define abs(x)  ((x) >= 0 ? (x) : (-x))
#define norm(x)  ((short)((x)/shrink+0.5))
#define normx(x) ((short)((x)/shrink+0.5))
/*
#define normy(y) ((short)((YLENG-(y))/shrink+0.5))
*/
#define direction(x) {}

Cord pb;

static int sp = 1;
static int cw = FCW, ch = FCH, th = 0;

extern Display *display;
extern Window main_window;
extern int screen;
extern GC gc;
extern unsigned long forepix, backpix;
extern float shrink;
extern int landscape;
extern int c_flg, mono_flg;
extern int fno;

static XPoint points[SIZE];
static int line_width = 1;
static int line_style = LineSolid;
static int join_style = JoinMiter;
static XRectangle rect;


static char *symbol_upper = "ABGDEZHQIKLMNXOPRSTUFCYW";
static char *symbol_lower = "abgdezhqiklmnxoprstufcyw";
#define INFTY 165
#define SPACE 32

static void line(XPoint * points, int n);
static void fillpoly(XPoint * points, int type, int n);
static void reset_fill(void);
/*static void dplot(int density, short x, short y, short w, short h);*/
static void fillbox(int type, short x, short y, short w, short h);
static int text(char *s, int n, int fn);

/*
 *	Color name data
 */
static char *c_name[] = { "black", "black", "blue",
   "red", "medium sea green",
   "violet", "orange", "turquoise"
};

/*
 *	Line width data
 */
static int l_width[] = { 0, 1, 1, 2, 3, 4, 1, 2, 3, 4 };

/*
 *	Line style data
 */
static struct line_style_info {
   char list[8];
   int no;
} l_style[] = { { {
1, 1}, 2},                      /* Dummy for Solid */
{ {
1, 5}, 2},                      /* dotted2 */
{ {
1, 3}, 2},                      /* dotted */
{ {
4, 2}, 2},                      /* dashed */
{ {
7, 3}, 2},                      /* long_dashed  */
{ {
1, 1}, 2},                      /* Dummy for Solid */
{ {
8, 2, 2, 2}, 4},                /* dot_dashed */
{ {
10, 2, 2, 2}, 4},               /* dot_dashed2  */
{ {
8, 2, 1, 2, 1, 2}, 6},          /* double_dot_dashed */
{ {
6, 2, 2, 2, 2, 2}, 6}           /* double_dot_dashed2 */
};


/*
 * Mark bitmap data
 */
#define mark_width  9
#define mark_height 9

#include "marks/mark0.dat"
#include "marks/mark1.dat"
#include "marks/mark2.dat"
#include "marks/mark3.dat"
#include "marks/mark4.dat"
#include "marks/mark5.dat"
#include "marks/mark6.dat"
#include "marks/mark7.dat"
#include "marks/mark8.dat"
#include "marks/mark9.dat"
#include "marks/mark10.dat"
#include "marks/mark11.dat"
#include "marks/mark12.dat"
#include "marks/mark13.dat"
#include "marks/mark14.dat"
#include "marks/mark15.dat"

static char *mark_bits[] = {
   mark0_bits, mark1_bits, mark2_bits, mark3_bits,
   mark4_bits, mark5_bits, mark6_bits, mark7_bits,
   mark8_bits, mark9_bits, mark10_bits, mark11_bits,
   mark12_bits, mark13_bits, mark14_bits, mark15_bits
};


/*
 * Till Pattern
 */
#define till_width 4
#define till_height 4

#include "tills/till0.dat"
#include "tills/till1.dat"
#include "tills/till2.dat"
#include "tills/till3.dat"
#include "tills/till4.dat"
#include "tills/till5.dat"
#include "tills/till6.dat"
#include "tills/till7.dat"
#include "tills/till8.dat"
#include "tills/till9.dat"
#include "tills/till10.dat"
#include "tills/till11.dat"
#include "tills/till12.dat"
#include "tills/till13.dat"
#include "tills/till14.dat"
#include "tills/till15.dat"
#include "tills/till16.dat"

static char *till_bits[] = {
   till0_bits, till1_bits, till2_bits, till3_bits,
   till4_bits, till5_bits, till6_bits, till7_bits,
   till8_bits, till9_bits, till10_bits, till11_bits,
   till12_bits, till13_bits, till14_bits, till15_bits,
   till16_bits
};

static short normy(int y)
{
   if (!landscape)
      return ((short) ((YLENG - (y)) / shrink + 0.5));
   else
      return ((short) ((XLENG - (y)) / shrink + 0.5));
}

static void _flush(void)
{
   if (sp > 1) {
      /*  polylines(sp);
       */
      line(points, sp);
      points[0].x = points[sp - 1].x;
      points[0].y = points[sp - 1].y;
      sp = 1;
   }
}

static void _send(Cord * buf)
{
   if (sp == SIZE)
      _flush();

   points[sp].x = normx(buf->x);
   points[sp++].y = normy(buf->y);
}

static int _getcord(Cord * buf)
{
   int c;

   while ((c = getchar()) == '\n' || c == ' ');
   if (c) {
      ungetc(c, stdin);
      scanf("%d %d", &(buf->x), &(buf->y));
      return (1);
   } else
      return (0);
}

static void _line(void)
{
   while (_getcord(&pb))
      _send(&pb);

   _flush();
}


static void _move(int x, int y)
{
   points[0].x = normx(x);
   points[0].y = normy(y);
}


static void line(XPoint * points, int n)
{
   XDrawLines(display, main_window, gc, points, n, CoordModeOrigin);
}

static void polyline(XPoint * points, int frame, int fill, int n)
{
   if (fill != -1 && (fill %= 10) != 9) {
      fillpoly(points, fill + 6, n + 1);
      reset_fill();
   }
   if (frame)
      line(points, n + 1);
}

/*static int polyg(int type)
{
   int x, y, w, h;

   scanf("%d %d %d %d", &x, &y, &w, &h);
   dplot(type, normx(x), normy(y + h), norm(w), norm(h));

   return (0);
   }*/

#define LEVEL 256
#define POINTS 1024

 /*static void dplot(int density, short x, short y, short w, short h)
    {
    int n;
    int n_max, n_plot, flg[POINTS];
    int p;
    XPoint pos[POINTS];

    n_max = ++w * ++h;
    n_plot = (density * n_max) / LEVEL;

    #if defined(HAVE_MEMSET)
    memset(flg, 0, sizeof(*flg) * POINTS);
    #elif defined(HAVE_BZERO)
    bzero((char *) flg, sizeof(*flg) * POINTS);
    #endif

    for (n = 0; n < n_plot; n++) {
    p = (int) (n_max * (double) rand() / (double) INT_MAX);
    if (flg[p] == 0) {
    flg[p] = 1;
    pos[n].x = x + (short) p % w;
    pos[n].y = y - (short) p / w;
    }
    }
    XDrawPoints(display, main_window, gc, pos, n_plot, CoordModeOrigin);
    } */

static void hatching(int type)
{
   int n;
   int frame;
   int d, angle;

   scanf("%d %d", &d, &angle);

   for (n = 0; _getcord(&pb); n++) {
      points[n].x = normx(pb.x);
      points[n].y = normy(pb.y);
   }
   points[n].x = points[0].x;
   points[n].y = points[0].y;

   switch (type -= 20) {
   case 1:
      frame = 1;
      type = -1;
      break;
   case 2:
      frame = 0;
      type = -1;
      break;
   case 3:
      frame = 1;
      type = -1;
      break;
   default:
      if (type < 0) {
         frame = 0;
         type = -type;
      } else
         frame = 1;
      break;
   }
   polyline(points, frame, type, n);
}

static void reset_fill(void)
{
   fillbox(15, 0, 0, 0, 0);
}

/*static void box(short x, short y, short w, short h)
{
   XDrawRectangle(display, main_window, gc, x, y, w, h);
   }*/

static void fillbox(int type, short x, short y, short w, short h)
{
   Pixmap till_pmap;

   till_pmap = XCreatePixmapFromBitmapData(display, main_window,
                                           till_bits[type], till_width,
                                           till_height, forepix, backpix,
                                           DefaultDepth(display, 0));

   XSetTile(display, gc, till_pmap);
   XSetFillStyle(display, gc, FillTiled);
   XFillRectangle(display, main_window, gc, x, y, w, h);

   XFreePixmap(display, till_pmap);
}

static void fillpoly(XPoint * points, int type, int n)
{
   Pixmap till_pmap;

   till_pmap = XCreatePixmapFromBitmapData(display, main_window,
                                           till_bits[type], till_width,
                                           till_height, forepix, backpix,
                                           DefaultDepth(display, 0));

   XSetTile(display, gc, till_pmap);
   XSetFillStyle(display, gc, FillTiled);
   XFillPolygon(display, main_window, gc, points, n, Convex, CoordModeOrigin);

   XFreePixmap(display, till_pmap);
}

static void get_str(void)
{
   int c, i, j, k, sfg = 0;
   char s[512];
   float adj = 0.29;

   for (i = j = k = 0; (c = getchar()); i++, j++) {
      if ((c &= 0xff) <= 192) { /*  not Symbol   */
         if (sfg) {             /*  flush buffer  */
            s[i] = '\0';
            points[0].x += (int) (cw * k * adj);
            text(s, i, FSymbol);
            k = j;
            i = j = 0;
            sfg = 0;
         }
         s[i] = c;
      } else {                  /*  Symbol        */
         if (!sfg) {            /*  flush buffer  */
            s[i] = '\0';
            points[0].x += (int) (cw * k * adj);
            text(s, i, fno);
            k = j;
            i = j = 0;
            sfg = 1;
         }
         if (c <= 216)
            sprintf(s + i, "%c", symbol_upper[c - 193]);
         else if (c >= 225 && c <= 248)
            sprintf(s + i, "%c", symbol_lower[c - 225]);
         else if (c == 254)
            sprintf(s + i, "%c", INFTY);
         else
            sprintf(s + i, "%c", SPACE);
      }
   }
   s[i] = '\0';
   points[0].x += (int) (cw * k * adj);
   if (!sfg)
      text(s, i, fno);
   else
      text(s, i, FSymbol);
}

static int text(char *s, int n, int fn)
{
   int cx, cy;
   static int flg = 0, cfn = -1, ccw = -1, cch = -1;
   float xadj, yadj;

   if (n <= 0)
      return (0);

   if (fn != cfn) {
      ccw = cw;
      cch = ch;
      if (ch == LFCH)
         cfn = fn + 1;
      else
         cfn = fn;
      flg = 1;
   }
   if (flg || cw != ccw || ch != cch) {
      if (cw < ccw || ch < cch)
         cfn = fn - 1;
      else if (cw > ccw || ch > cch)
         cfn = fn + 1;

      XSetFont(display, gc, XLoadFont(display, f_name[cfn]));
      ccw = cw;
      cch = ch;
      flg = 0;
   }

   if (th == 0)
      XDrawString(display, main_window, gc, points[0].x, points[0].y, s, n);
   else {
      xadj = ccw / shrink;
      yadj = ((cch > FCH) ? 0.75 : 1.25) * cch / shrink;
      while (n) {
         cx = points[0].x - xadj;
         cy = points[0].y - yadj * --n;
         XDrawString(display, main_window, gc, cx, cy, s, 1);
         s++;
      }
   }

   return 0;
}

static void newpen(int w)
{
   if (w < 0 || w > 10)
      w = 1;

   if (!c_flg || mono_flg) {
      line_width = l_width[w];
      XSetLineAttributes(display, gc,
                         line_width, line_style, CapButt, join_style);
   } else
      XSetForeground(display, gc, get_color_pix(c_name[w]));
}

static void join_type(int w)
{
   switch (w) {
   case 0:
      join_style = JoinMiter;
      break;
   case 1:
      join_style = JoinRound;
      break;
   case 2:
      join_style = JoinBevel;
      break;
   default:
      join_style = JoinMiter;
   }
}

static int line_type(int w)
{
   int dash_offset = 0;

   if (w == 0)
      line_style = LineSolid;
   else if (w > 0 && w < 12)
      line_style = LineOnOffDash;
   else
      return (0);

   XSetLineAttributes(display, gc, line_width, line_style, CapButt, join_style);
   if (w > 0) {
      XSetDashes(display, gc, dash_offset, l_style[w].list, l_style[w].no);
   }

   return (0);
}

static void clip(int xmin, int ymin, int xmax, int ymax)
{
   rect.x = xmin;
   rect.y = ymin;
   rect.width = xmax - xmin;
   rect.height = ymax - ymin;

   XSetClipRectangles(display, gc, 0, 0, &rect, 1, Unsorted);
}

static void mark(int w)
{
   Pixmap mark_pmap;

   mark_pmap = XCreatePixmapFromBitmapData(display, main_window,
                                           mark_bits[w], mark_width,
                                           mark_height, forepix, backpix,
                                           DefaultDepth(display, 0));

   XCopyArea(display, mark_pmap, main_window, gc,
             0, 0, mark_width, mark_height,
             points[0].x - mark_width / 2, points[0].y - mark_height / 2);

   XFreePixmap(display, mark_pmap);
}

static void circle(int x0, int y0, int r1, int r2, int arg1, int arg2)
{
   int x, y;
   unsigned int width, height;

   arg1 /= 10;
   arg2 /= 10;
   width = normx(abs(r1 * cos((double) arg1)));
   height = normx(abs(r2 * sin((double) arg2)));
   x = normx(x0) - width;
   y = normy(y0) - height;
   width *= 2;
   height *= 2;

   XDrawArc(display, main_window, gc, x, y,
            width, height, arg1 * 64, arg2 * 64);
}

void plot(void)
{
   int c;
   int w, n, xmin, ymin, xmax, ymax;
   int x0, y0, r1, r2, arg1, arg2;

   while ((c = getchar()) != EOF) {
      switch (c) {
      case 'M':
         scanf("%d %d", &pb.x, &pb.y);
         _move(pb.x, pb.y);
         break;
      case 'D':
         _line();
         break;
      case '%':
         scanf("%d", &n);
         hatching(n);
         break;
         /*    polyg(n);    break;
          */
      case 'P':
         get_str();
         break;
      case 'S':
         scanf("%d", &ch);
         break;
      case 'Q':
         scanf("%d", &cw);
         break;
      case 'R':
         scanf("%d", &th);
         th = (th == 0) ? 0 : 1;
         break;
      case 'L':
         scanf("%d", &w);
         line_type(w);
         break;
      case 'K':
         scanf("%d", &w);
         join_type(w);
         break;
      case 'W':
         scanf("%d %d %d %d %d %d", &x0, &y0, &r1, &r2, &arg1, &arg2);
         circle(x0, y0, r1, r2, arg1, arg2);
         break;
      case 'N':
         scanf("%d", &w);
         mark(w);
         break;
      case 'J':
         scanf("%d", &w);
         newpen(w);
         break;
      case '\\':
         scanf("%d %d", &xmin, &ymin);
         break;
      case 'Z':
         scanf("%d %d", &xmax, &ymax);
         if (!landscape) {
            if (xmax > XLENG)
               xmax = XLENG;
            if (ymax > YLENG)
               ymax = YLENG;
         } else {
            if (xmax > YLENG)
               xmax = YLENG;
            if (ymax > XLENG)
               ymax = XLENG;
         }
         clip(normx(xmin), normy(ymax), normx(xmax), normy(ymin));
         break;
      case ';':
      case ':':
         break;
      default:
         break;
      }
   }
}
