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


/********************************************************
* $Id: plot.c,v 1.21 2013/12/16 09:02:02 mataki Exp $  *
*       Interpret X-Y Plotter Commands                  *
********************************************************/

#include <stdio.h>
#include <string.h>
#include "psgr.h"

#define BUFSIZE 2048
#define MADJ 0.275
#define PU_PT (72.0/254.0)

#define norm(x)  (int)(x)
#define moveto(x, y) printf("%d %d MT\n", (x), (y))
#define lineto(x, y) printf("%d %d LT\n", (x), (y))
#define rmoveto(x, y) printf("%d %d RMT\n", (x), (y))
#define rlineto(x, y) printf("%d %d RLT\n", (x), (y))
#define clip_on() printf("GS\n")
#define clip_off() printf("GR\n")
#define circle(x, y, r1, r2, ang1, ang2)\
   arc((x), (y), (r1), (ang1), (ang2), 0)

static char *lmode[] = {
   "[] 0",                      /*  Line 0: Solid */
   "[1 16] 0",                  /*  Line 6: Dotted2 */
   "[1 7] 0",                   /*  Line 2: Dotted */
   "[8 8] 0",                   /*  Line 4: Dashed */
   "[16 14] 0",                 /*  Line 5: Long_Dashed */
   "[20 8 3 8] 0",
   "[18 7 2 7] 0",              /*  Line 3: Dot_Dashed */
   "[16 6 2 6 2 6] 0",
   "[20 8 3 8 3 8] 0"
};

static float lwidth[] = { 0.05, 2.00, 0.05, 2.00, 4.00, 6.00,
   0.01, 1.00, 0.10, 0.20, 0.40, 0.60
};

static char *fillpattern[] = {
   "0", "0.875",
   "P2", "P3", "P4", "P5", "P6", "P7",
   "0.75", "1",
};

static char *symbol_up = "ABGDEZHQIKLMNXOPRSTUFCYW";
static char *symbol_lw = "abgdezhqiklmnxoprstufcyw";
#define INFTY 165
#define SPACE 32
static char *font[] = { "FnS", "FnC", "FnCO", "FnCB", "FnCBO" };
static float fnmag[] = { 1.375, 1.50, 1.50, 1.50, 1.50 };

Cord pb;

static int xb[BUFSIZE], yb[BUFSIZE];
static int sp = 1;
static int ch = 30, cw = 30, th = 0, rotate = 0, mfg = 0;
static int init_str_flag = 0;   /*  Initial string segment
                                   flag for text rotation. */

static int _getcord(FILE * fp, struct cord *buf)
{
   int c;

   while ((c = getc(fp)) == '\n' || c == ' ');
   if (c) {
      ungetc(c, fp);
      fscanf(fp, "%d %d", &(buf->x), &(buf->y));
      return (1);
   } else
      return (0);
}

void polylines(int *x, int *y, int n)
{
   moveto(*x++, *y++);
   while (--n > 0)
      lineto(*x++, *y++);

   printf("ST\n");
}

static void _flush(void)
{
   if (sp > 1) {
      polylines(xb, yb, sp--);
      xb[0] = xb[sp];
      yb[0] = yb[sp];
      sp = 1;
   }

   return;
}

static void _send(struct cord *buf)
{
   if (sp == BUFSIZE)
      _flush();
   xb[sp] = norm(buf->x);
   yb[sp++] = norm(buf->y);

   return;
}

static void rectangle(int x0, int y0, int x1, int y1, int frame, int fill)
{
   static char gray[16][8] = {
      "0", "0.875",
      "0.7", "0.6", "0.5", "0.4", "0.3", "0.2",
      "0.75", "1",
   };

   fill %= 10;
   if (fill >= 2 && fill <= 7) {
      printf("%s SG\n", gray[fill]);
      /*  printf("PM {%s} IM\n", fillpattern[fill]);
       */
      printf("%d %d %d %d RP\n", (x0), (y0), (x1 - x0), (y1 - y0));
   } else
      printf("%s SG\n", fillpattern[fill]);

   printf("%d %d %d %d RF\n", (x0), (y0), (x1 - x0), (y1 - y0));

   if (fill != 0)
      printf("0 SG\n");

   if (frame)
      printf("%d %d %d %d RS\n", (x0), (y0), (x1 - x0), (y1 - y0));
}

static void text(char *s, int n, int fn)
{
   int fn_w, fn_h;
   static int cfn = -1, ccw = -1, cch = -1;

   if (n <= 0)
      return;

   if (cfn != fn || ccw != cw || cch != ch || mfg) {
      fn_w = (int) (cw * fnmag[fn]);
      fn_h = (int) (ch * fnmag[fn]);

      if (cw == ch)
         if (fn != 0)           /*  not Symbol  */
            printf("%s %d SF\n", font[fn], fn_w);
         else
            printf("%s [%d 0 %d %d 0 0] SF\n", font[fn], fn_w, fn_h / 4, fn_h);
      else if (fn != 0)
         printf("%s [%d 0 0 %d 0 0] SF\n", font[fn], fn_w, fn_h);
      else
         printf("%s [%d 0 %d %d 0 0] SF\n", font[fn], fn_w, fn_h / 4, fn_h);

      cfn = fn;
      ccw = cw;
      cch = ch;
      mfg = 0;
   }
   if (th == 0)
      printf("(%s) %d %d MS\n", s, xb[0], yb[0]);
   else {
      if (init_str_flag) {
         printf("%d %d MT\n", xb[0], yb[0]);
         printf("%d rotate\n", rotate);
         printf("(%s) show\n", s);
         printf("%d rotate\n", -rotate);
         init_str_flag = 0;
      } else {
         printf("%d %d TR\n", xb[0], yb[0]);
         printf("%d rotate\n", rotate);
         printf("(%s) show\n", s);
         printf("%d rotate\n", -rotate);
         printf("%d %d TR\n", -xb[0], -yb[0]);
      }
   }
}

static void rect(int x, int y, int w, int h, int fill)
{
   if (!fill)
      printf("%d %d %d %d RS\n", x, y, w, h);
   else
      printf("%d %d %d %d RF\n", x, y, w, h);
}

static void tangle(int size, int fill)
{
   int bsize;

   bsize = (int) (0.8 * size);
   rmoveto(0, bsize);
   rlineto(size, -2 * bsize);
   rlineto(-2 * size, 0);
   printf("closepath\n");
   if (fill)
      printf("fill\n");
   printf("ST\n");
}

static void arc(int x, int y, int r, int ang1, int ang2, int fill)
{
   printf("newpath\n");
   printf("%d %d %d %d %d arc\n", x, y, r, ang1, ang2);
   printf("closepath\n");
   if (fill)
      printf("fill\n");
   printf("ST\n");
}

static void symbol(char *code, int size, int xoffset, int yoffset)
{
   static char prev[16];

   if (!mfg || strcmp(prev, code)) {
      printf("FnS %d SF\n", size);
      mfg = 1;
      strcpy(prev, code);
   }
   printf("(%s) %d %d RMS\n", code, xoffset, yoffset);
}

static void _move(int x, int y)
{
   xb[0] = norm(x);
   yb[0] = norm(y);
}

static void _line(FILE * fp)
{
   while (_getcord(fp, &pb))
      _send(&pb);

   _flush();
}

/*static void intstyle(int type)
{
   int is_frame = 1, style = 2;

   switch (type -= 20) {
   case 1:
      style = 0;
      break;
   case 2:
      is_frame = 0;
      break;
   case 3:
      break;
   default:
      if (type < 0)
         is_frame = 0;
      else
         type = -type;
      if ((style = type + 10) == 0)
         style = 1;
   }
   }*/

/*static void polyg(FILE * fp, int type)
{
   int n;
   int d, angle;

   intstyle(type);
   fscanf(fp, "%d %d", &d, &angle);
   for (n = 1; _getcord(fp, &pb); n++) {
      xb[n] = norm(pb.x);
      yb[n] = norm(pb.y);
   }
   xb[n] = xb[1];
   yb[n] = yb[1];
   polylines(xb + 1, yb + 1, n);

   return;
   }*/

static void hatching(FILE * fp, int type)
{
   int n;
   int frame;
   int d, angle;

   fscanf(fp, "%d %d", &d, &angle);

   for (n = 1; _getcord(fp, &pb); n++) {
      xb[n] = norm(pb.x);
      yb[n] = norm(pb.y);
   }
   xb[n] = xb[1];
   yb[n] = yb[1];

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
   if (type == -1)
      polylines(xb + 1, yb + 1, n);
   else
      rectangle(xb[1], yb[1], xb[2], yb[3], frame, type);
}

/*
#define box(x0, y0, x1, y1)\
   printf("%d %d %d %d RS\n",(x0),(y0),(x1-x0),(y1-y0))
#define fillbox(x0, y0, x1, y1)\
   printf("%d %d %d %d RF\n",(x0),(y0),(x1-x0),(y1-y0))
#define setfill(x) printf("%3.1f SG\n", (x))
#define unsetfill() printf("0 SG\n")

rectangle(x0, y0, x1, y1, frame, fill)
 int x0, y0, x1, y1;
 int frame, fill;
{
 if ((fill %= 10)!=9)  {
  if (fill)  {
   setfill(gray[fill]);
   fillbox(x0, y0, x1, y1);
   unsetfill();
  }
  else
   fillbox(x0, y0, x1, y1);
 }

 if (frame)
  box(x0, y0, x1, y1);
}
*/
/*
hatching(fp, type)
 FILE *fp;
 int type;
{
 int n;
 int  style;
 int  x, y, d, ang, r1, r2, ang1, ang2;

 style = type % 10;
 type /= 10;

 switch (type)  {
     case 0:
   fscanf(fp, "%d %d %d %d", &x, &y, &d, &ang);
   box(x, y);
   break;
     case 1:
   fscanf(fp, "%d %d %d %d %d %d",
      &r1, &r2, &ang1, &ang2, &d, &ang);
   circle(norm(pb.x), norm(pb.y), r1, r2, ang1, ang2);
   break;
     case 2:
   for (n=1; _getcord(fp, &pb); n++)  {
    xb[n] = norm(pb.x);
    yb[n] = norm(pb.y);
   }
   xb[n] = xb[1];
   yb[n] = yb[1];
   polylines(xb+1, yb+1, n);
   break;
     default:
   break;
 }
}
*/

static void get_str(FILE * fp)
{
   int c;                       /*  charactor code  */
   int i;                       /*  outputs strings length  */
   int j;                       /*  previus strings length of same fonts  */
   int k;                       /*  current strings length of same fonts  */
   int sfg = 0;                 /*  Symbol fonts' flag  */
   char s[512];
   static float mag = 0.875;    /*  7/8  */

   init_str_flag = 1;

   for (i = j = k = 0; (c = getc(fp)); i++, j++) {
      if ((c &= 0xff) <= 192) { /*  not Symbol   */
         if (sfg) {             /*  flush buffer  */
            s[i] = '\0';
            xb[0] += (int) (cw * k * mag);
            text(s, i, 0);
            k = j;
            i = j = 0;
            sfg = 0;
         }
         if (c != '(' && c != ')')
            s[i] = c;
         else {
            sprintf(s + i, "\\%c", c);
            i++;
         }
      } else {                  /*  Symbol        */
         if (!sfg) {            /*  flush buffer  */
            s[i] = '\0';
            xb[0] += (int) (cw * k * mag);
            text(s, i, font_no);
            k = j;
            i = j = 0;
            sfg = 1;
         }
         if (c <= 216)
            sprintf(s + i, "%c", symbol_up[c - 193]);
         else if (c >= 225 && c <= 248)
            sprintf(s + i, "%c", symbol_lw[c - 225]);
         else if (c == 254)
            sprintf(s + i, "%c", INFTY);
         else
            sprintf(s + i, "%c", SPACE);
      }
   }
   s[i] = '\0';
   xb[0] += (int) (cw * k * mag);
   if (!sfg)
      text(s, i, font_no);
   else
      text(s, i, 0);
}

static void line_type(int type)
{
   if (type < 0 || type > 8)
      type = 0;

   printf("%s setdash\n", lmode[type]);
}

static void join_type(int type)
{
   if (type < 0 || type > 2)
      type = 0;

   printf("%d setlinejoin\n", type);
}

static void newpen(int width)
{
   if (width < 0 || width > 10)
      width = 1;

   printf("%4.2f SL\n", lwidth[width]);
}

static void mark(int no)
{
   int size, dsize, qsize, hsize;

   /* size = (int)(ch * MADJ);
    */
   size = (int) (ch * MADJ * 0.75);
   dsize = 2 * size;
   qsize = 4 * size;
   hsize = size / 2;

   moveto(*xb, *yb);
   switch (no) {
   case 1:
      arc(*xb, *yb, hsize, 0, 360, 1);
      break;
   case 2:
      symbol("\264", dsize, -hsize, -hsize);
      break;
   case 3:
      rect(*xb - size, *yb - size, dsize, dsize, 0);
      break;
   case 4:
      tangle(size, 0);
      break;
   case 5:
      arc(*xb, *yb, size, 0, 360, 0);
      break;
   case 6:
      symbol("\340", qsize, -size, -size - hsize);
      break;
   case 7:
      symbol("\264", qsize, -size, -size);
      break;
   case 8:
      symbol("\053", qsize, -size, -size);
      break;
   case 9:
      symbol("\304", qsize, -size * 8 / 5, -size * 7 / 5);
      break;
   case 10:
      symbol("\305", qsize, -size * 8 / 5, -size * 7 / 5);
      break;
   case 11:
      rect(*xb - size, *yb - size, dsize, dsize, 1);
      break;
   case 12:
      tangle(size, 1);
      break;
   case 13:
      arc(*xb, *yb, size, 0, 360, 1);
      break;
   case 14:
      symbol("\250", qsize, -size - hsize, -size);
      break;
   case 15:
      symbol("\052", qsize, -size, -size - hsize);
      break;
   default:
      break;
   }
}



static void clipping(int x0, int y0, int x1, int y1)
{
   if (clip_mode) {
      clip_off();
      clip_mode = 0;
   }

   if ((x0 != 0) && (y0 != 0) && (x1 != xleng) && (y1 != yleng)) {
      clip_on();
      clip_mode = 1;

      printf("%d %d %d %d RC\n", x0, y0, x1 - x0, y1 - y0);
   }
}


void plot(FILE * fp)
{
   int c;
   int n, xmin, xmax, ymin, ymax;
   int r1, r2, ang1, ang2;

   while ((c = getc(fp)) != EOF) {
      switch (c) {
      case 'M':
         fscanf(fp, "%d %d", &pb.x, &pb.y);
         _move(pb.x, pb.y);
         break;
      case 'D':
         _line(fp);
         break;
         /*  case '%': fscanf(fp, "%d", &n); polyg(fp, n); break;
          */
      case '%':
         fscanf(fp, "%d", &n);
         hatching(fp, n);
         break;
       /**/ case 'P':
         get_str(fp);
         break;
      case 'S':
         fscanf(fp, "%d", &ch);
         ch += 3;
         break;
      case 'Q':
         fscanf(fp, "%d", &cw);
         cw += 3;
         break;
      case 'R':
         fscanf(fp, "%d", &th);
         rotate = th / 10;
         th = (th == 0) ? 0 : 1;
         break;
      case 'L':
         fscanf(fp, "%d", &n);
         line_type(n);
         break;
      case 'K':
         fscanf(fp, "%d", &n);
         join_type(n);
         break;
      case 'W':
         fscanf(fp, "%d %d %d %d %d %d", &pb.x, &pb.y, &r1, &r2, &ang1, &ang2);
         circle(norm(pb.x), norm(pb.y), r1, r2, ang1, ang2);
         break;
      case 'N':
         fscanf(fp, "%d", &n);
         mark(n);
         break;
      case 'J':
         fscanf(fp, "%d", &n);
         newpen(n);
         break;
      case '\\':
         fscanf(fp, "%d %d", &xmin, &ymin);
         if (xmin < 0)
            xmin = 0;
         if (ymin < 0)
            ymin = 0;
         break;
      case 'Z':
         fscanf(fp, "%d %d", &xmax, &ymax);
         if (xmax > xleng)
            xmax = xleng;
         if (ymax > yleng)
            ymax = yleng;
         clipping(norm(xmin), norm(ymin), norm(xmax), norm(ymax));
         break;
      case ';':
      case ':':
         break;
      default:
         break;
      }
   }
}
