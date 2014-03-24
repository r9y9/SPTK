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


/****************************************************************
*         XY-Plotter Library                                    * 
*                                                               *
*                                        1985    T. Kobayashi   *
*                                        2010.9  A. Tamamori    *
*                                                               *
*         Calling sequence :                                    *
*                 plopen(mode);                                 *
*                 _plsend(buf, nbytes);                         *
*                 _plnorm(x);                                   *
*                 plots(mode);                                  *
*                 plote();                                      *
*                 plot(x, y, z);                                *
*                 plotr(ip, x, y);                              *
*                 plota(ip, x, y);                              *
*                 origin(x, y);                                 *
*                 mode(ltype, lscale);                          *
*                 chlnmod(_lnmode);                             *
*                                                               *
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if defined(WIN32)
#  include <windows.h>
#  define sleep(x) Sleep(x)
#else
#  include <unistd.h>
#endif
#include "plot.h"

struct co_ord _org = { 0.0, 0.0 };

void plots(int mode)
{
   if ((mode & 0x80) == 0) {
      asciimode();              /* ASCII mode */
      ifclear();                /* Interface CLEAR */
      sleep(1);                 /* delay */
      home();                   /* HOME */
   }
   term(TRM);
   _org.xo = _org.yo = 0.0;
}

void plote(void)
{
   return;
}

int _plnorm(double x)
{
   return ((int) (10.0 * x + 0.5));
}

int _cordx(double x)
{
   return (_plnorm(x + _org.xo));
}

int _cordy(double y)
{
   return (_plnorm(y + _org.yo));
}

static short _ltype = 0, _lscale = 10;

void plot(double x, double y, int z)
{
   int ip;

   if (abs(z) == 2)
      ip = 0;
   else if (abs(z) == 3)
      ip = 1;
   else {
      if (z == 999)
         plote();
      return;
   }
   plota(ip, x, y);
   if (z < 0) {
      _org.xo += x;
      _org.yo += y;
   }
}

int plotr(int ip, double x, double y)
{
   if (ip == 1)
      rmove(_plnorm(x), _plnorm(y));
   else if (ip == 0 || ip == 2) {
      if (ip == 2)
         _chlnmod(1);
      rdraw(_plnorm(x), _plnorm(y));
      if (ip == 2)
         _chlnmod(0);
   } else
      return (1);
   return (0);
}

int plota(int ip, double x, double y)
{
   if (ip == 1)
      move(_cordx(x), _cordy(y));
   else if (ip == 0 || ip == 2) {
      if (ip == 2)
         _chlnmod(1);
      draw(_cordx(x), _cordy(y));
      if (ip == 2)
         _chlnmod(0);
   } else
      return (1);
   return (0);
}

void mode(int ltype, double lscale)
{
   _ltype = ltype;
   _lscale = _plnorm(lscale);
}

void _chlnmod(int lmode)
{
   if (lmode) {
      line_type(_ltype);
      line_scale(_lscale);
   } else
      line_type(0);
}

void origin(double x, double y)
{
   _org.xo = x;
   _org.yo = y;
   plot(0., 0., 3);
}

void org(double x, double y)
{
   _org.xo = x;
   _org.yo = y;
}
