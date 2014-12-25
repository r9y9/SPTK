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

/*******************************************************************************
*                                                                              *
*    Execute Scalar Operations                                                 *
*                                                                              *
*                                     1990.11 T.Kobayashi                      *
*                                     1996.5  K.Koishida                       *
*                                     2000.5  T.Kobayashi                      *
*                                     2010.6  A.Tamamori                       *
*                                     2010.12 T.Sawada                         *
*                                     2013.3  T.Okada                          *
*       usage:                                                                 *
*               sopr [ options ] [ infile ] > stdout                           *
*       options:                                                               *
*               -a a         :  addition             (in + a)                  *
*               -s s         :  subtraction          (in - s)                  *
*               -m m         :  multiplication       (in * m)                  *
*               -d d         :  division             (in / d)                  *
*               -p p         :  power                (in ^ p)                  *
*               -f f         :  flooring             (in < f -> f)             *
*               -c c         :  ceiling              (in > f -> f)             *
*               -ABS         :  absolute             (abs(in))                 *
*               -INV         :  inverse              (1 / in)                  *
*               -P           :  square               (in * in)                 *
*               -R           :  root                 (sqrt(in))                *
*               -SQRT        :  root                 (sqrt(in))                *
*               -LN          :  logarithm natural    (log(in))                 *
*               -LOG2        :  logarithm to base 2  (log2(in))                *
*               -LOG10       :  logarithm to base 10 (log10(in))               *
*               -LOGX X      :  logarithm to base X  (logX(in))                *
*               -EXP         :  exponential          (exp(in))                 *
*               -POW2        :  power of 2           (2^(in))                  *
*               -POW10       :  power of 10          (10^(in))                 *
*               -POWX X      :  power of X           (X^(in))                  *
*               -FIX         :  round                ((int)in)                 *
*               -UNIT        :  unit step            (u(in))                   *
*               -CLIP        :  clipping             (in * u(in)               *
*               -SIN         :  sin                  (sin(in))                 *
*               -COS         :  cos                  (cos(in))                 *
*               -TAN         :  tan                  (tan(in))                 *
*               -ATAN        :  atan                 (atan(in))                *
*                                                                              *
*               -magic magic :  remove magic number                            *
*               -MAGIC MAGIC :  replace magic number by MAGIC                  *
*                               if -magic option is not given,                 *
*                               return error                                   *
*                               if -magic or -MAGIC option                     *
*                               is given multiple times, return error          *
*               -r mn    :  read from memory register n                        *
*               -w mn    :  write to memory register n                         *
*                                                                              *
*      infile:                                                                 *
*               data sequences (float)                                         *
*      stdout:                                                                 *
*               data sequences after operations                                *
*                                                                              *
*******************************************************************************/

static char *rcs_id = "$Id: sopr.c,v 1.44 2014/12/11 08:30:50 uratec Exp $";


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

#include <ctype.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Command Name  */
char *cmnd;


/* Default Value  */
#define MEMSIZE  10
#define MAGIC    FA
#define REP      FA

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - execute scalar operations\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -a a         : addition             (in + a)\n");
   fprintf(stderr, "       -s s         : subtraction          (in - s)\n");
   fprintf(stderr, "       -m m         : multiplication       (in * m)\n");
   fprintf(stderr, "       -d d         : division             (in / d)\n");
   fprintf(stderr, "       -p p         : power                (in ^ p)\n");
   fprintf(stderr,
           "       -f f         : flooring             (f if in < f)\n");
   fprintf(stderr,
           "       -c c         : ceiling              (c if in > c)\n");
   fprintf(stderr, "       -magic magic : remove magic number  \n");
   fprintf(stderr, "       -MAGIC MAGIC : replace magic number by MAGIC\n");
   fprintf(stderr, "                      if -magic option is not given,\n");
   fprintf(stderr, "                      return error\n");
   fprintf(stderr,
           "       if -magic or -MAGIC option is given multiple times,\n");
   fprintf(stderr, "       return error\n");
   fprintf(stderr, "\n");
   fprintf(stderr,
           "       if the argument of the above operation option is `dB', `cent',\n");
   fprintf(stderr,
           "       `semitone' or `octave', then the value 20/log_e(10), 1200/log_e(2),\n");
   fprintf(stderr,
           "       12/log_e(2) or 1/log_e(2) is assigned, respectively. Also if `pi' or\n");
   fprintf(stderr,
           "       `ln(x)',`exp(x)',`sqrt(x)' such as `ln2',`exp10',`sqrt30' \n");
   fprintf(stderr,
           "       is written after the operation option, then its value\n");
   fprintf(stderr, "       will be used\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "       -ABS         : absolute             (abs(in))\n");
   fprintf(stderr, "       -INV         : inverse              (1 / in)\n");
   fprintf(stderr, "       -P           : square               (in * in)\n");
   fprintf(stderr, "       -R           : root                 (sqrt(in))\n");
   fprintf(stderr, "       -SQRT        : root                 (sqrt(in))\n");
   fprintf(stderr, "       -LN          : logarithm natural    (log(in))\n");
   fprintf(stderr, "       -LOG2        : logarithm to base 2  (log2(in))\n");
   fprintf(stderr, "       -LOG10       : logarithm to base 10 (log10(in))\n");
   fprintf(stderr, "       -LOGX X      : logarithm to base X  (logX(in))\n");
   fprintf(stderr, "       -EXP         : exponential          (exp(in))\n");
   fprintf(stderr, "       -POW2        : power of 2           (2^(in))\n");
   fprintf(stderr, "       -POW10       : power of 10          (10^(in))\n");
   fprintf(stderr, "       -POWX X      : power of X           (X^(in))\n");
   fprintf(stderr, "       -FIX         : round                ((int)in)\n");
   fprintf(stderr, "       -UNIT        : unit step            (u(in))\n");
   fprintf(stderr, "       -CLIP        : clipping             (in * u(in))\n");
   fprintf(stderr, "       -SIN         : sin                  (sin(in))\n");
   fprintf(stderr, "       -COS         : cos                  (cos(in))\n");
   fprintf(stderr, "       -TAN         : tan                  (tan(in))\n");
   fprintf(stderr, "       -ATAN        : atan                 (atan(in))\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "       -r mn        : read from memory register n\n");
   fprintf(stderr, "       -w mn        : write to memory register n\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "       -h           : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence (%s)        [stdin]\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       data sequence after operations (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

#define LOG2(x) (log(x)/log(2))

struct operation {
   char op[4];
   double d;
   Boolean magic;
   Boolean ifrep;
} *optbl;
int nopr = 0;
int mopr = 0;
int ropr = 0;

static double mem[MEMSIZE];

int main(int argc, char *argv[])
{
   int magic_count = 0, rep_count = 0;
   FILE *fp;
   char *s, c;
   char *infile = NULL;
   int sopr(FILE *);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   optbl = (struct operation *) calloc(sizeof(struct operation), argc);

   for (; --argc;) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if (islower(c) && *++s == '\0') {
            s = *++argv;
            if ((s == NULL) && (c != 'h')) {
               fprintf(stderr,
                       "%s : numerical argument is also needed !\n", cmnd);
               usage(1);
            }
            --argc;
         }

         switch (c) {
         case 'a':
         case 'c':
         case 'd':
         case 'f':
         case 'p':
         case 'P':
         case 'm':
         case 's':
         case 'r':
         case 'w':
         case 'M':
         case 'L':
            if (c == 'P') {
               if (strncmp("POWX\0", s, 5) == 0) {
                  strncpy(optbl[nopr].op, "POWX", 4);
                  s = *++argv;
                  if (s == NULL) {
                     fprintf(stderr,
                             "%s : numerical argument is also needed !\n",
                             cmnd);
                     usage(1);
                  }
                  --argc;
               } else {
                  strncpy(optbl[nopr].op, s, 4);
               }
            }
            if (c == 'L') {
               if (strncmp("LOGX\0", s, 5) == 0) {
                  strncpy(optbl[nopr].op, "LOGX", 4);
                  s = *++argv;
                  if (s == NULL) {
                     fprintf(stderr,
                             "%s : numerical argument is also needed !\n",
                             cmnd);
                     usage(1);
                  }
                  if (atof(s) <= 0) {
                     fprintf(stderr,
                             "%s : base of a logarithm must be positive number !\n",
                             cmnd);
                     usage(1);
                  }
                  --argc;
               } else {
                  strncpy(optbl[nopr].op, s, 4);
               }
            }
            if ((c == 'm') && strncmp("agic", s, 4) == 0) {
               if (magic_count > 0) {
                  fprintf(stderr,
                          "%s : Cannot specify -magic option multiple times!\n",
                          cmnd);
                  usage(1);
               }
               optbl[nopr].magic = 1 - MAGIC;
               mopr = nopr;
               magic_count++;
               s = *++argv;
               if (s == NULL) { /* No magic number */
                  fprintf(stderr,
                          "%s : -magic option need magic number !\n", cmnd);
                  usage(1);
               }
               --argc;
            }
            if (c == 'M') {
               if (rep_count > 0) {
                  fprintf(stderr,
                          "%s : Cannot specify -MAGIC option multiple times!\n",
                          cmnd);
                  usage(1);
               }
               if (!optbl[mopr].magic) {
                  fprintf(stderr,
                          "%s : Cannot find -magic option befor -MAGIC option!\n",
                          cmnd);
                  usage(1);
               } else {
                  optbl[nopr].magic = 1 - MAGIC;
                  optbl[nopr].ifrep = 1 - REP;
                  rep_count++;
                  s = *++argv;
                  if (s == NULL) {      /* No magic number */
                     fprintf(stderr,
                             "%s : -MAGIC option need magic number !\n", cmnd);
                     usage(1);
                  }
                  --argc;
               }
            }
            if (strncmp("dB", s, 2) == 0)
               optbl[nopr].d = 20 / log(10.0);
            else if (strncmp("cent", s, 4) == 0)
               optbl[nopr].d = 1200 / log(2.0);
            else if (strncmp("octave", s, 6) == 0)
               optbl[nopr].d = 1.0 / log(2.0);
            else if (strncmp("semitone", s, 8) == 0)
               optbl[nopr].d = 12.0 / log(2.0);
            else if (strncmp("pi", s, 2) == 0)
               optbl[nopr].d = PI;
            else if (strncmp("ln", s, 2) == 0)
               optbl[nopr].d = log(atof(s + 2));
            else if (strncmp("exp", s, 3) == 0)
               optbl[nopr].d = exp(atof(s + 3));
            else if (strncmp("sqrt", s, 4) == 0)
               optbl[nopr].d = sqrt(atof(s + 4));
            else if (*s == 'm') {
               s = *(argv + 1);
               if (s == NULL) {
                  fprintf(stderr,
                          "%s : next operation must be specified !\n", cmnd);
                  usage(1);
               }
               optbl[nopr].d = atoi(s + 1);
               if (c == 'a')
                  c = '+';
               else if (c == 'd')
                  c = '/';
               else if (c == 'm')
                  c = '*';
               else if (c == 's')
                  c = '-';
               else if (c == 'p')
                  c = '^';
            } else {
               if (isdigit(*s) == 0 && strncmp(optbl[nopr].op, s, 4) != 0) {    /* Check the value is correct */
                  if ((*s) != '+' && (*s) != '-') {
                     fprintf(stderr,
                             "%s : %s option need numerical number !\n", cmnd,
                             *(argv - 1));
                     usage(1);
                  } else if (isdigit(*(s + 1)) == 0) {
                     fprintf(stderr,
                             "%s : %s option need numerical number !\n", cmnd,
                             *(argv - 1));
                     usage(1);
                  }
               }
               optbl[nopr].d = atof(s);
            }
         case 'A':
         case 'C':
         case 'E':
         case 'F':
         case 'I':
         case 'R':
         case 'S':
         case 'T':
         case 'U':
            if ((c == 'A') || (c == 'C') || (c == 'S'))
               strncpy(optbl[nopr].op, s, 4);
            else
               optbl[nopr].op[0] = c;
            ++nopr;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, c);
            usage(1);
         }
      } else
         infile = s;
   }
   if (infile) {
      fp = getfp(infile, "rb");
      sopr(fp);
      infile = NULL;
   } else
      sopr(stdin);

   return (0);
}

int sopr(FILE * fp)
{
   double x, y;
   int k, i;
   Boolean skipflg = FA;

   while (freadf(&x, sizeof(x), 1, fp) == 1) {
      for (k = 0; k < MEMSIZE; ++k)
         mem[k] = 0;
      for (k = 0; k < nopr; ++k) {
         y = optbl[k].d;
         if (optbl[k].magic) {  /* -magic or -MAGIC */
            if (optbl[k].ifrep) {       /* -MAGIC */
               if (x == optbl[mopr].d) {        /* still remains magic number */
                  x = y;        /* substitute by new magic number */
                  skipflg = FA;
               }
            } else if (x == y) {        /* -magic */
               skipflg = TR;
            }
         } else if (skipflg == FA) {
            switch (optbl[k].op[0]) {
            case 'r':
               x = mem[(int) y];
               break;
            case 'w':
               mem[(int) y] = x;
               break;
            case '+':
               x += mem[(int) y];
               break;
            case '-':
               x -= mem[(int) y];
               break;
            case '*':
               x *= mem[(int) y];
               break;
            case '/':
               x /= mem[(int) y];
               break;
            case '^':
               x = pow(x, mem[(int) y]);
               break;
            case 'a':
               x += y;
               break;
            case 's':
               x -= y;
               break;
            case 'm':
               x *= y;
               break;
            case 'd':
               x /= y;
               break;
            case 'p':
               x = pow(x, y);
               break;
            case 'f':
               x = (x < y) ? y : x;
               break;
            case 'c':
               x = (x > y) ? y : x;
               break;
            case 'A':
               if (optbl[k].op[1] == 'T')
                  x = atan(x);
               else if (x < 0)
                  x = -x;
               break;
            case 'C':
               if (optbl[k].op[1] == 'L') {
                  if (x < 0)
                     x = 0;
               } else
                  x = cos(x);
               break;
            case 'I':
               x = 1 / x;
               break;
            case 'P':
               if (optbl[k].op[1] == 'O' && optbl[k].op[3] == '1')
                  x = pow(10.0, x);
               else if (optbl[k].op[1] == 'O' && optbl[k].op[3] == '2')
                  x = pow(2.0, x);
               else if (optbl[k].op[1] == 'O' && optbl[k].op[3] == 'X')
                  x = pow(y, x);
               else
                  x *= x;
               break;
            case 'R':
               x = sqrt(x);
               break;
            case 'S':
               if (optbl[k].op[1] == 'Q')
                  x = sqrt(x);
               else
                  x = sin(x);
               break;
            case 'E':
               x = exp(x);
               break;
            case 'L':
               if (x <= 0)
                  fprintf(stderr, "WARNING: LOG of zero or negative value !\n");
               if (optbl[k].op[3] == 'X')
                  x = log(x) / log(y);
               else if (optbl[k].op[3] == '1')
                  x = log10(x);
               else if (optbl[k].op[3] == '2')
                  x = LOG2(x);
               else
                  x = log(x);
               break;
            case 'F':
               if (x < 0)
                  i = x - 0.5;
               else
                  i = x + 0.5;
               x = i;
               break;
            case 'T':
               x = tan(x);
               break;
            case 'U':
               if (x < 0)
                  x = 0;
               else
                  x = 1;
            case 'M':

            default:
               break;
            }
         }
      }
      if (skipflg == FA)
         fwritef(&x, sizeof(x), 1, stdout);
      skipflg = FA;
   }
   return (0);
}
