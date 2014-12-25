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

/**************************************************************************
*                                                                         *
*    Execute Vector Operations                                            *
*                                                                         *
*                                               1988.6  T.Kobayashi       *
*                                               1996.5  K.Koishida        *
*       usage:                                                            *
*               vopr [ options ] [ [ file1 ] [ infile ]  > stdout         *
*       options:                                                          *
*               -l l     :  length of vector                [1]           *
*               -n n     :  order of vector                 [l-1]         *
*               -i       :  specified file contains a and b [FALSE]       *
*               -a       :  addition        (a + b)         [FALSE]       *
*               -s       :  subtraction     (a - b)         [FALSE]       *
*               -m       :  multiplication  (a * b)         [FALSE]       *
*               -d       :  division        (a / b)         [FALSE]       *
*               -ATAN2   :  atan2           atan2(b,a)      [FALSE]       *
*               -AM      :  arithmetic mean ((a + b) / 2)   [FALSE]       *
*               -GM      :  geometric mean  (sqrt(a * b))   [FALSE]       *
*               -c       :  choose smaller value            [FALSE]       *
*               -f       :  choose larger value             [FALSE]       *
*               -gt      :  decide 'greater than'           [FALSE]       *
*               -ge      :  decide 'greater than or equal'  [FALSE]       *
*               -lt      :  decide 'less than'              [FALSE]       *
*               -le      :  decide 'less than or equal'     [FALSE]       *
*               -eq      :  decide 'equal to'               [FALSE]       *
*               -ne      :  decide 'not equal to'           [FALSE]       *
*        notice:                                                          *
*               When both -l and -n are specified,                        *
*               latter argument is adopted.                               *
*               When -gt, -ge, -le, -lt, -eq or -ne is specified,         *
*               each element of output vec. is 1.0 (true) or 0.0 (false). *
*                                                                         *
***************************************************************************/

static char *rcs_id = "$Id: vopr.c,v 1.28 2014/12/11 08:30:51 uratec Exp $";


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
#include <ctype.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define LENG 1
#define INV  FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - execute vector operations\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ file1 ] [ infile ] > stdout\n",
           cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -l l  : length of vector                              [%d]\n",
           LENG);
   fprintf(stderr,
           "       -n n  : order of vector                               [l-1]\n");
   fprintf(stderr,
           "       -i    : specified file contains a and b               [%s]\n",
           BOOL[INV]);
   fprintf(stderr,
           "       -a    : addition                       (a + b)        [FALSE]\n");
   fprintf(stderr,
           "       -s    : subtraction                    (a - b)        [FALSE]\n");
   fprintf(stderr,
           "       -m    : multiplication                 (a * b)        [FALSE]\n");
   fprintf(stderr,
           "       -d    : division                       (a / b)        [FALSE]\n");
   fprintf(stderr,
           "       -ATAN : atan2                          atan(b, a)     [FALSE]\n");
   fprintf(stderr,
           "       -AM   : arithmetic mean                ((a + b) / 2)  [FALSE]\n");
   fprintf(stderr,
           "       -GM   : geometric  mean                (sqrt(a * b))  [FALSE]\n");
   fprintf(stderr,
           "       -c    : choose smaller value                          [FALSE]\n");
   fprintf(stderr,
           "       -f    : choose larger value                           [FALSE]\n");
   fprintf(stderr,
           "       -gt   : decide 'greater than'          (a > b)        [FALSE]\n");
   fprintf(stderr,
           "       -ge   : decide 'greater than or equal' (a >= b)       [FALSE]\n");
   fprintf(stderr,
           "       -lt   : decide 'less than'             (a < b)        [FALSE]\n");
   fprintf(stderr,
           "       -le   : decide 'less than or equal'    (a <= b)       [FALSE]\n");
   fprintf(stderr,
           "       -eq   : decide 'equal to'              (a == b)       [FALSE]\n");
   fprintf(stderr,
           "       -ne   : decide 'not equal to'          (a != b)       [FALSE]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       data vectors (%s)                                  [stdin]\n",
           FORMAT);
   fprintf(stderr, "  file1:\n");
   fprintf(stderr, "       data vectors (%s)\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       data vectors after operations (float)\n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       When both -l and -n are specified,\n");
   fprintf(stderr, "       latter argument is adopted.\n");
   fprintf(stderr, "\n");
   fprintf(stderr,
           "       When -gt, -ge, -le, -lt, -eq or -ne is specified,\n");
   fprintf(stderr,
           "       each element of output vectors is 1.0 (true) or 0.0 (false).\n");

#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}


int opr = ' ', leng = LENG;
Boolean inv = INV;

int main(int argc, char **argv)
{
   int nfiles = 0;
   FILE *fp1, *fp2;
   char *s, c, *infile[4];
   int vopr(FILE * fp1, FILE * fp2);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc) {
      if (*(s = *++argv) == '-') {
         c = *++s;
         if (*++s == '\0' && (c == 'l' || c == 'n')) {
            s = *++argv;
            --argc;
         }
         switch (c) {
         case 'l':
            if ((c == 'l') && strncmp("e", s, 1) == 0) {
               opr = 'r';
            } else if ((c == 'l') && strncmp("t", s, 1) == 0) {
               opr = 'R';
            } else {
               leng = atoi(s);
            }
            break;
         case 'n':
            if ((c == 'n') && strncmp("e", s, 1) == 0) {
               opr = 'E';
            } else {
               leng = atoi(s) + 1;
            }
            break;
         case 'i':
            inv = 1 - inv;
            break;
         case 'a':
         case 'd':
         case 'm':
         case 's':
         case 'A':
         case 'c':
         case 'f':
         case 'g':
         case 'G':
         case 'e':
            opr = c;
            if ((c == 'A') && strncmp("M", s, 1) == 0) {
               opr = 'p';
            } else if ((c == 'G') && strncmp("M", s, 1) == 0) {
               opr = 'P';
            } else if ((c == 'g') && strncmp("e", s, 1) == 0) {
               opr = 'q';
            } else if ((c == 'g') && strncmp("t", s, 1) == 0) {
               opr = 'Q';
            }
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         infile[nfiles++] = s;
   }

   if (nfiles == 0)
      vopr(stdin, stdin);
   else {
      fp1 = getfp(infile[0], "rb");
      if (nfiles == 1) {
         if (inv)
            vopr(fp1, fp1);
         else
            vopr(stdin, fp1);
      } else {
         fp2 = getfp(infile[1], "rb");
         vopr(fp1, fp2);
      }
   }

   return (0);
}

int vopr(FILE * fp1, FILE * fp2)
{
   double *a, *b;
   int k;

   a = dgetmem(leng + leng);
   b = a + leng;

   if (fp1 != fp2 && leng > 1) {
      if (freadf(b, sizeof(*b), leng, fp2) != leng)
         return (1);
   }
   while (freadf(a, sizeof(*a), leng, fp1) == leng) {
      if (fp1 == fp2 || leng == 1) {
         if (freadf(b, sizeof(*b), leng, fp2) != leng)
            return (1);
      }

      switch (opr) {
      case 'a':
         for (k = 0; k < leng; ++k)
            a[k] += b[k];
         break;
      case 's':
         for (k = 0; k < leng; ++k)
            a[k] -= b[k];
         break;
      case 'm':
         for (k = 0; k < leng; ++k)
            a[k] *= b[k];
         break;
      case 'd':
         for (k = 0; k < leng; ++k)
            a[k] /= b[k];
         break;
      case 'A':
         for (k = 0; k < leng; ++k)
            a[k] = atan2(b[k], a[k]);
         break;
      case 'c':                /* choose smaller one */
         for (k = 0; k < leng; ++k) {
            if (a[k] > b[k]) {
               a[k] = b[k];
            }
         }
         break;
      case 'f':                /* choose larger one */
         for (k = 0; k < leng; ++k) {
            if (a[k] < b[k]) {
               a[k] = b[k];
            }
         }
         break;
      case 'p':                /* arithmetic mean */
         for (k = 0; k < leng; ++k)
            a[k] = (a[k] + b[k]) / 2;
         break;
      case 'P':                /* geometric mean */
         for (k = 0; k < leng; ++k) {
            double tmp = a[k] * b[k];
            if (tmp < 0.0) {
               fprintf(stderr, "%s : Can't calculate geometric mean !\n", cmnd);
               usage(1);
            }
            a[k] = sqrt(tmp);
         }
         break;
      case 'q':                /* greater than or equal */
         for (k = 0; k < leng; ++k) {
            if (a[k] >= b[k]) {
               a[k] = 1.0;
            } else {
               a[k] = 0.0;
            }
         }
         break;
      case 'Q':                /* greater than */
         for (k = 0; k < leng; ++k) {
            if (a[k] > b[k]) {
               a[k] = 1.0;
            } else {
               a[k] = 0.0;
            }
         }
         break;
      case 'r':                /* less than or equal */
         for (k = 0; k < leng; ++k) {
            if (a[k] <= b[k]) {
               a[k] = 1.0;
            } else {
               a[k] = 0.0;
            }
         }
         break;
      case 'R':                /* less than */
         for (k = 0; k < leng; ++k) {
            if (a[k] < b[k]) {
               a[k] = 1.0;
            } else {
               a[k] = 0.0;
            }
         }
         break;
      case 'e':                /* equal to */
         for (k = 0; k < leng; ++k) {
            if (a[k] == b[k]) {
               a[k] = 1.0;
            } else {
               a[k] = 0.0;
            }
         }
         break;
      case 'E':                /* not equal to */
         for (k = 0; k < leng; ++k) {
            if (a[k] != b[k]) {
               a[k] = 1.0;
            } else {
               a[k] = 0.0;
            }
         }
         break;
      default:
         break;
      }

      fwritef(a, sizeof(*a), leng, stdout);
   }
   return (0);
}
