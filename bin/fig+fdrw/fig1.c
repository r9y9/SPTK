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
* NAME :         fig1 - subroutines for fig                     *
****************************************************************/

#include <string.h>
#include <math.h>
#include "fig.h"
#include "plot.h"

/*  Required Functions  */

#define GRKSIZE 62
static char *grk[] = { "Alpha", "Beta", "Gamma", "Delta",
   "Epsilon", "Zeta", "Eta", "Theta",
   "Iota", "Kappa", "Lambda", "Mu",
   "Nu", "Xi", " ", "Pi",
   "Rho", "Sigma", "Tau", "Upsilon",
   "Phi", "Chi", "Psi", "Omega",
   " ", " ", " ", " ", " ", " ", " ", " ",
   "alpha", "beta", "gamma", "delta",
   "epsilon", "zeta", "eta", "theta",
   "iota", "kappa", "lambda", "mu",
   "nu", "xi", " ", "pi",
   "rho", "sigma", "tau", "upsilon",
   "phi", "chi", "psi", "omega",
   " ", " ", " ", " ", " ", "infty"
};

static char sub[SBUFLNG];
static int nsub;

char *gettxt_fig(char *s)
{
   char *p;
   int i, c;
   char *script(char *s, int i);

   while (*s && *s != '"')
      ++s;
   if (*s++ == '\0')
      return (s - 1);
   for (p = s, nsub = i = 0; *s && (*s != '"' || *(s + 1) == '"'); ++s) {
      if (*s == '\\') {         /* escape sequence */
         if ((c = greek(s + 1)) >= 0) {
            s += strlen(grk[c]);
            c += 193;
         } else {
            switch (c = *++s) {
            case 'b':
               c = 0x08;
               break;
            case 'n':
               c = 0x0a;
               break;
            case 'r':
               c = 0x0d;
               break;
            case '^':
               c = '^';
               break;
            case '_':
               c = '_';
               break;
            case '\\':
               c = '\\';
               break;
            case 'd':
            case 'u':
               s = script(s, i);
               c = ' ';
               break;
            default:
               sscanf(s, "%3d", &c);
               s += 2;
               break;
            }
         }
         p[i++] = c;
      } else if ((c = *s) == '^' || c == '_') {
         if (*(s + 1) == '{') {
            for (*++s = c; *(s + 1) != '}'; p[i++] = ' ') {
               s = script(s, i);
               *s = c;
            }
            ++s;
         } else {
            s = script(s, i);
            p[i++] = ' ';
         }
      } else {
         if (*s == '"')
            ++s;
         p[i++] = *s;
      }
   }
   sub[nsub] = p[i] = '\0';
   while (p + i < s)
      p[++i] = ' ';
   return (p);
}

char *script(char *s, int i)
{
   int c;

   sub[nsub++] = *s;
   sub[nsub++] = i;
   if ((c = *++s) == '\\') {
      if ((c = greek(s + 1)) >= 0) {
         s += strlen(grk[c]);
         c += 193;
      } else {
         switch (c = *++s) {
         case '\\':
         case '{':
         case '}':
         case '_':
         case '^':
            break;
         default:
            sscanf(s, "%3d", &c);
            s += 2;
            break;
         }
      }
   }
   sub[nsub++] = c;
   sub[nsub++] = '\0';
   return (s);
}

char *getarg(char *s, char *arg)
{
   if (s == NULL)
      return (s);
   while (*s == ' ' || *s == '\t' || *s == '\n')
      ++s;
   if (*s == '\0')
      return (NULL);
   else if (*s == '"') {
      gettxt_fig(s);
      while ((*arg++ = *s++));
   } else {
      while (*s != ' ' && *s != '\t' && *s != '\n' && *s != '\0')
         *arg++ = *s++;
      *arg = '\0';
   }
   return (s);
}

char *gettyp(char *s, char *t)
{
   char *p;

   s = getarg(p = s, t);
   if (isalpha(*t))
      return (s);
   else {
      strcpy(t, "lin");
      return (p);
   }
}

char *getname(char *s, char *t)
{
   if ((s = getarg(s, t)) == NULL)
      *t = '\0';
   else if (*t == '"')
      *t++ = '\0';
   else {
      if (!is_number(*t))
         *t = '\0';
      t = gettxt_fig(s);
   }
   return (t);
}

int greek(char *p)
{
   int n;

   for (n = 0; n < GRKSIZE; ++n)
      if (strncmp(p, grk[n], strlen(grk[n])) == 0)
         return (n);
   return (-1);
}

double sleng(char *p, double h, double w)
{
   int len = strlen(p);
   double ret = (len - 1) * w + LADJ * w;

   return (ret);
}

void _symbol(double x, double y, char *p, double h, double w, double t)
{
   int i;
   double dx, dy;

   symbol(x, y, p, h, w, t);
   for (i = 0; sub[i]; i += 4) {
      dy = (sub[i] == 'd' || sub[i] == '_') ? -h * 0.25 : h * 0.7;
      dx = sub[i + 1] * w;
      symbol(x + rx(dx, dy, t), y + ry(dx, dy, t), sub + i + 2, h * SSIZE, w,
             t);
   }
   sub[0] = 0;
}

double ysadj(void)
{
   int i;

   for (i = 0; sub[i]; i += 4)
      if (sub[i] == 'u' || sub[i] == '^')
         return (1.5);

   return (0);
}

double rx(double x, double y, double t)
{
   t *= (3.141592653589793 / 180);
   return (x * cos(t) - y * sin(t));
}

double ry(double x, double y, double t)
{
   t *= (3.141592653589793 / 180);
   return (x * sin(t) + y * cos(t));
}

#define PI 3.141592653589793
#include <math.h>

double argapf(double x, double a)
{
   double omg;

   omg = PI * x;
   return (x + 2.0 * atan2(a * sin(omg), 1.0 - a * cos(omg)) / PI);
}
