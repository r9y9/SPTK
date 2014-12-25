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

/************************************************************************
*                                                                       *
*    Data Merge                                                         *
*                                                                       *
*                                       1986.6  K.Tokuda                *
*                                       1996.5  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               merge [options] file1 [infile] > stdout                 *
*       options:                                                        *
*               -s s     :  insert point                        [0]     *
*               -l l     :  frame length of input data          [25]    *
*               -n n     :  order of input data                 [l-1]   *
*               -L L     :  frame length of insert data         [10]    *
*               -N N     :  order of insert data                [L-1]   *
*               -o       :  over write mode                     [FALSE] *
*               +type    :  data type                           [f]     *
*                           c (char)           C  (unsigned char)       *
*                           s (short)          S  (unsigned short)      *
*                           i (int)            I  (unsigned int)        *
*                           i3 (int, 3byte)    I3 (unsigned int, 3byte) *
*                           l (long)           L  (unsigned long)       *
*                           le (long long)     LE (unsigned long long)  *
*                           f (float)          d  (double)              *
*                           de (long double)                            *
*       file1:  inserted data   , x(0), x(1), ..., x(l-1)               *
*       file2:  input data      , y(0), y(1), ..., y(n-1)               *
*       stdout:                                                         *
*               x(0), ..., x(s), y(0), ...,y(n-1), x(s+1), ..., x(n-1)  *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: merge.c,v 1.32 2014/12/11 08:30:41 uratec Exp $";


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

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define START 0
#define LENG1 25
#define LENG2 10
#define WRITE FA

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - data merge\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] file1 [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -s s  : insert point                [%d]\n", START);
   fprintf(stderr, "       -l l  : frame length of input data  [%d]\n", LENG1);
   fprintf(stderr, "       -n n  : order of input data         [l-1]\n");
   fprintf(stderr, "       -L L  : frame length of insert data [%d]\n", LENG2);
   fprintf(stderr, "       -N N  : order of insert data        [L-1]\n");
   fprintf(stderr, "       -o    : over write mode             [%s]\n",
           BOOL[WRITE]);
   fprintf(stderr, "       +type : data type                   [f]\n");
   fprintf(stderr,
           "                c  (char, %lubyte)         C  (unsigned char, %lubyte)\n",
           (unsigned long) sizeof(char), (unsigned long) sizeof(unsigned char));
   fprintf(stderr,
           "                s  (short, %lubyte)        S  (unsigned short, %lubyte)\n",
           (unsigned long) sizeof(short),
           (unsigned long) sizeof(unsigned short));
   fprintf(stderr,
           "                i3 (int, 3byte)          I3 (unsigned int, 3byte)\n");
   fprintf(stderr,
           "                i  (int, %lubyte)          I  (unsigned int, %lubyte)\n",
           (unsigned long) sizeof(int), (unsigned long) sizeof(unsigned int));
   fprintf(stderr,
           "                l  (long, %lubyte)         L  (unsigned long, %lubyte)\n",
           (unsigned long) sizeof(long), (unsigned long) sizeof(unsigned long));
   fprintf(stderr,
           "                le (long long, %lubyte)    LE (unsigned long long, %lubyte)\n",
           (unsigned long) sizeof(long long),
           (unsigned long) sizeof(unsigned long long));
   fprintf(stderr,
           "                f  (float, %lubyte)        d  (double, %lubyte)\n",
           (unsigned long) sizeof(float), (unsigned long) sizeof(double));
   fprintf(stderr, "                de (long double, %lubyte)\n",
           (unsigned long) sizeof(long double));
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence                       [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       merged data sequence\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

int main(int argc, char **argv)
{
   FILE *fp2 = NULL, *fp1 = stdin;
   int start = START, leng1 = LENG1, leng2 = LENG2, i, j, flag = 1;
   size_t size = sizeof(float);
   Boolean write = WRITE;
   char *y, c, *s;
   long double x;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 's':
            start = atoi(*++argv);
            --argc;
            break;
         case 'l':
            leng1 = atoi(*++argv);
            --argc;
            break;
         case 'n':
            leng1 = atoi(*++argv) + 1;
            --argc;
            break;
         case 'L':
            leng2 = atoi(*++argv);
            --argc;
            break;
         case 'N':
            leng2 = atoi(*++argv) + 1;
            --argc;
            break;
         case 'o':
            write = 1 - write;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (*s == '+') {
         c = *++s;
         switch (c) {
         case 'c':
            size = sizeof(char);
            break;
         case 's':
            size = sizeof(short);
            break;
         case 'i':
            if (*(s + 1) == '3') {
               size = 3;
               (*argv)++;
            } else {
               size = sizeof(int);
            }
            break;
         case 'l':
            if (*(s + 1) == 'e') {
               size = sizeof(long long);
               (*argv)++;
            } else {
               size = sizeof(long);
            }
            break;
         case 'C':
            size = sizeof(unsigned char);
            break;
         case 'S':
            size = sizeof(unsigned short);
            break;
         case 'I':
            if (*(s + 1) == '3') {
               size = 3;
               (*argv)++;
            } else {
               size = sizeof(unsigned int);
            }
            break;
         case 'L':
            if (*(s + 1) == 'E') {
               size = sizeof(unsigned long long);
               (*argv)++;
            } else {
               size = sizeof(unsigned long);
            }
            break;
         case 'f':
            size = sizeof(float);
            break;
         case 'd':
            if (*(s + 1) == 'e') {
               size = sizeof(long double);
               (*argv)++;
            } else {
               size = sizeof(double);
            }
            break;
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (fp2 == NULL)
         fp2 = getfp(*argv, "rb");
      else
         fp1 = getfp(*argv, "rb");

   if (fp2 == NULL) {
      fprintf(stderr, "%s : Inserted data must be specified !\n", cmnd);
      usage(1);
   }

   y = (char *) dgetmem(leng2 * size);

   for (;;) {
      for (j = start, i = leng1; j-- && i--;) {
         if (freadx(&x, size, 1, fp1) != 1)
            break;
         fwritex(&x, size, 1, stdout);
      }
      for (j = leng2; j--;)
         if (write) {
            if (freadx(&x, size, 1, fp1) != 1)
               break;
            i--;
         }
      if (freadx(y, size, leng2, fp2) != leng2)
         if (!flag)
            break;

      fwritex(y, size, leng2, stdout);
      flag = 0;
      for (; i-- > 0;) {
         if (freadx(&x, size, 1, fp1) != 1)
            break;
         fwritex(&x, size, 1, stdout);
      }
   }

   if ((fgetc(fp1) == EOF) && (fgetc(fp2) == EOF)) {
      if (feof(fp1) && feof(fp2))
         return (0);
   }
   return (1);
}
