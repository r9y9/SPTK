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
* $Id: plot.h,v 1.18 2014/12/11 08:30:35 uratec Exp $          *  
* HEADER for XY-PLOTTER library                                 *
*                                                               *
*    Following commands are valid for the xy plotter FP5301     *
*                                                               *
*                                               Oct. 1, 1985    *
****************************************************************/

#include <stdio.h>

/* commands for FP5301 */

#define TRM 0                   /* terminator */

#define _speed(l, n) printf("!%d %d%c", l, n, TRM)      /* speed */
#define _speed_all(l) printf("!%d%c", l, TRM)   /* speed */
#define ERRMASK 0x8022          /* error mask  */
#define _font(n) printf("$%d%c", n, TRM)        /* font */
#define _hatch(m,p,t) printf("%%%d %d %d\n", m, p, t)   /* hatching */
#define _factor(p,q,r) printf("&%d %d %d\n", p, q, r)   /* factor */
#define ELLIPS 0x8029           /* ellips  */
#define _rotate(x,y,t) printf("/%d %d %d\n", x, y, t)   /* rotate */
#define clear()  putchar(':')   /* clear */
#define ifclear() putchar(';')  /* interface clear */
#define term(t)  printf("=%c%c", t, t)  /* term */
#define CLIP 0x803e             /* clipping  */
#define RDOFF 0x803f            /* read offset  */
#define ARESET 0x8041           /* alpha reset  */
#define line_scale(l) printf("B%d\n", l)        /* line scale */
#define draw(x, y) printf("D%d %d\n%c", x, y, TRM)      /* draw */
#define _draw()  putchar('D')   /* draw command */
#define rdraw(x, y) printf("E%d %d\n%c", x, y, TRM)     /* relative draw */
#define home();  putchar('H')   /* home */
#define aitalic(p) printf("I%d\n", p)   /* alpha italic */
#define newpen(p) printf("J%d\n", p)    /* new pen */
#define join_type(p) printf("K%d\n", p) /* join type */
#define KANA 0x804b             /* kana   */
#define line_type(p) printf("L%d\n", p) /* line type */
#define move(x, y) printf("M%d %d\n", x, y)     /* move */
#define _mark(m) printf("N%d\n", m)     /* mark */
#define rmove(x, y) printf("O%d %d\n", x, y)    /* relative move */
#define print(p) printf("P%s%c", p, TRM)        /* print */
#define aspace(l) printf("Q%d\n", l)    /* alpha space */
#define arotate(t) printf("R%d\n", t)   /* alpha rotate */
#define ascale(a) printf("S%d\n", a)    /* alpha scale */
#define PROMPT 0x8054           /* prompt light  */
#define _circle(x,y,q,r,s,t) printf("W%d %d %d %d %d %d%c",x,y,q,r,s,t,TRM)
/* circle */
#define _axis(p,q,r,s,t) printf("X%d %d %d %d %d%c", p,q,r,s,t,TRM)
/* axis */
#define _curve(a) printf("Y%d\n", a)    /* curve */
#define wup_left(x, y) printf("Z%d %d\n", x, y) /* write upper right */
#define wlo_right(x, y) printf("\\%d %d\n", x, y)       /* write lower left */
#define _rcircle(q,r,s,t) printf("]%d %d %d %d%c",q,r,s,t,TRM)
/* relative circle */
#define _offset(x, y) printf("^%d %d\n", x, y)  /* offset */
#define RCURVE 0x805f           /* relative curve */
#define asciimode() printf("%c%c", 0x80, 0)     /* binary terminator */
#define terminate() putchar(TRM)        /* terminator */
#define sndcord(x, y) printf("%d %d\n", x, y)   /* send (x, y) */

/* interfacing */

struct co_ord {
   double xo;
   double yo;
};

#define sign(x)  (x & 0xffff8000)
#define DEG_RAD  (3.141592653589793 / 180.0)
#define SCALE 4096

/*  Required Functions  */
void rotate(double th);
void factor(double fx, double fy);
int offset(double x, double y);
void bound(double xl, double yl, double xh, double yh);
void rstbnd(double w, double h, double fct);
int hatch(int ip, double *ax, double *ay, int n, double d, double t);
int pen(int pen);
int join(int join);
int font(int n);
int speed(int isp, int ip);
int mark(int mrk, double ax[], double ay[], int n, double f, int m);
int symbol(double x, double y, char *text, double h, double s, double th);
int number(double x, double y, double fval, double h, double s, double th,
           int m, int n);
int italic(double th);
int line(int ip, double *ax, double *ay, int n);
int circle(double x, double y, double rs, double re, double ths, double the);
int pntstyl(int ip);
void plots(int mode);
void plote();
int _plnorm(double x);
int _cordx(double x);
int _cordy(double y);
void plot(double x, double y, int z);
int plotr(int ip, double x, double y);
int plota(int ip, double x, double y);
void mode(int ltype, double lscale);
void _chlnmod(int lmode);
void origin(double x, double y);
void org(double x, double y);
void graph(FILE * fp);
void draw_fig0(double x[], double y[], int n, double w, double h, double fct);
int flush(double x[], double y[], int n, double w, double h, double fct);
void polyg(double x[], double y[], int n, double w, double h, double fct);
int is_in(double x, double y);
void swap(double *x, double *y);
char *gettxt_fig(char *s);
char *script(char *s, int i);
char *getarg(char *s, char *arg);
char *gettyp(char *s, char *t);
char *getname(char *s, char *t);
int greek(char *p);
double sleng(char *p, double h, double w);
void _symbol(double x, double y, char *p, double h, double w, double t);
double ysadj(void);
double rx(double x, double y, double t);
double ry(double x, double y, double t);
double argapf(double x, double a);
