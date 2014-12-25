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
*        XY PLOTTER library                                     *
*                                                               *
*                                        1985  T. Kobayashi     *
*                                                               *
*                   Calling sequence :                          *
*                   factor(fx, fy);                             *
*                   rotate(th);                                 *
*                   offset(x, y);                               *
*                   bound(xl, yl, xh, yh);                      *
*                   hatch(ip, x, y, n, d, t);                   *
*                   pen(ip);                                    *
*                   speed(isp);                                 *
*                   mark(mrk, ax, ay, n, f[, m]);               *
*                   symbol(x, y, text, h, s, th)                *
*                   number(x, y, fval, h, s, th, m, n);         *
*                   italic(th)                                  *
*                   font(n)                                     *
*                   line(ip, x, y, n);                          *
*                   circle(x, y, rs, re, ths, the)              *
*                   rcircle(rs, re, ths, the);                  *
*                                                               *
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "plot.h"

extern struct co_ord _org;

void rotate(double th)
{
   _rotate(_plnorm(_org.xo), _plnorm(_org.yo), _plnorm(th));
}

void factor(double fx, double fy)
{
   _factor((int) (fx * SCALE + 0.5), (int) (fy * SCALE + 0.5), SCALE);
}

int offset(double x, double y)
{
   _offset(_plnorm(x), _plnorm(y));
   return (0);
}

void bound(double xl, double yl, double xh, double yh)
{
   wlo_right(_cordx(xl), _cordy(yl));
   wup_left(_cordx(xh), _cordy(yh));
}

void rstbnd(double w, double h, double F)
{
   wlo_right(0, 0);
   wup_left((int) ((4000 * w + _org.xo * 10.0) * F),
            (int) ((2850 * h + _org.yo * 10.0) * F));
}

int hatch(int ip, double *ax, double *ay, int n, double d, double t)
{
   if (n < 3)
      return (1);

   ip += 20;
   _hatch(ip, _plnorm(d), _plnorm(t));

   while (--n >= 0)
      sndcord(_cordx(*ax++), _cordy(*ay++));
   terminate();
   return (0);
}

int pen(int pen)
{
   if (pen < 1 || pen > 10)
      return (1);
   newpen(pen);
   return (0);
}

int join(int join)
{
   if (join < 0 || join > 2)
      return (1);
   join_type(join);
   return (0);
}

int font(int n)
{
   if (n < 0 || n > 19)
      return (1);
   _font(n);
   return (0);
}

int speed(int isp, int ip)
{
   if (isp < 0 || isp > 10 || ip < 0 || ip > 10)
      return (1);
   if (ip)
      _speed_all(isp);
   else
      _speed(isp, ip);
   return (0);
}

int mark(int mrk, double ax[], double ay[], int n, double f, int m)
{
   int i;

   if (mrk < 0 || mrk > 15)
      return (1);
   m = (n < 0) ? m : 1;
   if ((n = abs(n)) < 1 || m < 0)
      return (1);
   ascale(_plnorm(f * 1.75));
   arotate(0);
   for (i = 0; i < n; i += m) {
      plot(ax[i], ay[i], 3);
      _mark(mrk);
   }
   return (0);
}

int symbol(double x, double y, char *text, double h, double s, double th)
{
   plot(x, y, 3);
   ascale(_plnorm(h));
   aspace(_plnorm(s));
   arotate(_plnorm(th));
   print(text);
   return (0);
}

int number(double x, double y, double fval, double h, double s, double th,
           int m, int n)
{
   char buf[32], format[8];

   if (abs(m) > 10)
      return (1);
   if (n <= 0) {
      while (++n < 0)
         fval /= 10.0;
      sprintf(format, "%%%dd%s", m, (n) ? ".\r" : "\r");
      sprintf(buf, format, (long) fval);
   } else {
      sprintf(format, "%%%d.%df\r", m, n);
      sprintf(buf, format, fval);
   }
   return (symbol(x, y, buf, h, s, th));
}

int italic(double th)
{
   int theta = 256 * tan(th * DEG_RAD);

   if (theta > 4000)
      return (1);
   aitalic(theta);
   return (0);
}

int line(int ip, double *ax, double *ay, int n)
{
   struct {
      short x;
      short y;
   } b, o, pb;
   int dx, dy;

   if (n < 2)
      return (1);
   if (ip)
      _chlnmod(1);
   plot(*ax++, *ay++, 3);
   o.x = dx = _cordx(*ax++);
   o.y = dy = _cordy(*ay++);
   pb = o;
   _draw();

   while (--n > 1) {
      dx = (b.x = _cordx(*ax++)) - pb.x;
      dy = (b.y = _cordy(*ay++)) - pb.y;
      if (dx || dy) {
         if (dx == 0 && o.x == 0 && sign(dy) == sign(o.y))
            pb.y = b.y;
         else if (dy == 0 && o.y == 0 && sign(dx) == sign(o.x))
            pb.x = b.x;
         else {
            sndcord(pb.x, pb.y);
            pb.x = b.x;
            pb.y = b.y;
         }
         o.x = dx;
         o.y = dy;
      }
   }
   sndcord(pb.x, pb.y);
   terminate();
   if (ip)
      _chlnmod(0);
   return (0);
}

int circle(double x, double y, double rs, double re, double ths, double the)
{
   int r1, r2;

   r1 = _plnorm(rs);
   r2 = _plnorm(re);
   if (r1 == 0 && r2 == 0)
      return (1);
   _circle(_cordx(x), _cordy(y), r1, r2, _plnorm(ths), _plnorm(the));
   return (0);
}

int pntstyl(int ip)
{
   ip += 20;
   aspace(ip);                  /* valid for only LBP */
   return (0);
}
