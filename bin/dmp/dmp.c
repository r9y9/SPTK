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

/************************************************************************
*                                                                       *
*    Binary File Dump                                                   * 
*                                                                       *
*                                       1996.5  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               dmp [options] [infile] > stdout                         *
*       options:                                                        *
*               -n n     :  block order  (0,...,n)           [EOD]      *
*               -l l     :  block length (1,...,l)           [EOD]      *
*               +type    :  data type                        [f]        *
*                           c (char)           C  (unsigned char)       *
*                           s (short)          S  (unsigned short)      *
*                           i (int)            I  (unsigned int)        *
*                           i3 (int, 3byte)    I3 (unsigned int, 3byte) *
*                           l (long)           L  (unsigned long)       *
*                           le (long long)     LE (unsigned long long)  *
*                           f (float)          d  (double)              *
*                           de (long double)                            *
*               %form    :  print format(printf style)       [N/A]      *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: dmp.c,v 1.28 2013/12/16 09:01:55 mataki Exp $";


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
#define SIZE 128


/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - binary file dump\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -n n  : block order   (0,...,n)      [EOD]\n");
   fprintf(stderr, "       -l l  : block length  (1,...,l)      [EOD]\n");
   fprintf(stderr, "       +type : data type                    [f]\n");
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
   fprintf(stderr, "       %%form : print format(printf style)   [N/A]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence                        [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       dumped sequence\n");
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
   int n = -1, i = 0, eflag = 0, lflag = 0, y;
   size_t size = sizeof(float);
   FILE *fp = stdin;
   char *s, c, cc = 'f';
   char format[SIZE], form[SIZE];
   int ff = 0;
   Boolean int3flg = FA, uint3flg = FA;
   union u {
      char c;
      short s;
      int i;
      int i3;
      long l;
      long long le;
      unsigned char C;
      unsigned short S;
      unsigned int I;
      unsigned int I3;
      unsigned long L;
      unsigned long long LE;
      float f;
      double d;
      long double de;
   } x;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc)
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 'n':
            n = atoi(*++argv) + 1;
            --argc;
            break;
         case 'l':
            n = atoi(*++argv) - 1;
            lflag = 1;
            --argc;
            break;
         case 'e':
            eflag = 1 - eflag;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (*s == '+') {
         if (*(s + 2) == '%') {
            strcpy(format, s + 2);
            ff = 1;
         }
         c = *++s;
         switch (c) {
         case 'c':
            cc = 'c';
            size = sizeof(char);
            break;
         case 's':
            cc = 's';
            size = sizeof(short);
            break;
         case 'i':
            if (*(s + 1) == '3') {
               size = 3;
               int3flg = TR;
               cc = 't';
               (*argv)++;
            } else {
               size = sizeof(int);
               cc = 'i';
            }
            break;
         case 'l':
            if (*(s + 1) == 'e') {
               size = sizeof(long long);
               cc = 'u';
               (*argv)++;
            } else {
               size = sizeof(long);
               cc = 'l';
            }
            break;
         case 'C':
            size = sizeof(unsigned char);
            cc = 'C';
            break;
         case 'S':
            size = sizeof(unsigned short);
            cc = 'S';
            break;
         case 'I':
            if (*(s + 1) == '3') {
               size = 3;
               uint3flg = TR;
               cc = 'T';
               (*argv)++;
            } else {
               size = sizeof(unsigned int);
               cc = 'I';
            }
            break;
         case 'L':
            if (*(s + 1) == 'E') {
               size = sizeof(unsigned long long);
               cc = 'U';
               (*argv)++;
            } else {
               size = sizeof(unsigned long);
               cc = 'L';
            }
            break;
         case 'f':
            size = sizeof(float);
            cc = 'f';
            break;
         case 'd':
            if (*(s + 1) == 'e') {
               size = sizeof(long double);
               cc = 'v';
               (*argv)++;
            } else {
               size = sizeof(double);
               cc = 'd';
            }
            break;
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (*s == '%') {
         strcpy(format, s);
         ff = 1;
      } else
         fp = getfp(*argv, "rb");

   for (i = 0;; i++) {
      if ((n >= 0) && (i >= n + lflag))
         i = 0;
      if (freadx(&x.f, size, 1, fp) != 1)
         break;
      if (eflag)
         printf("%d\t0\n", i + lflag);
      switch (cc) {
      case 'c':
         strcpy(form, "%d\t%d\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.c);
         break;
      case 's':
         strcpy(form, "%d\t%d\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.s);
         break;
      case 't':
         strcpy(form, "%d\t%d\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         y = x.i3 & 0x00FFFFFF;
         if (y >> 23 == 1)
            y = y | 0xFF000000;
         printf(form, i + lflag, y);
         break;
      case 'i':
         strcpy(form, "%d\t%d\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.i);
         break;
      case 'l':
         strcpy(form, "%d\t%d\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.l);
         break;
      case 'u':
         strcpy(form, "%d\t%lld\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.le);
         break;
      case 'C':
         strcpy(form, "%d\t%u\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.C);
         break;
      case 'S':
         strcpy(form, "%d\t%u\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.S);
         break;
      case 'I':
         strcpy(form, "%d\t%u\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.I);
         break;
      case 'T':
         strcpy(form, "%d\t%u\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         y = x.I3 & 0x00FFFFFF;
         printf(form, i + lflag, y);
         break;
      case 'L':
         strcpy(form, "%d\t%u\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.L);
         break;
      case 'U':
         strcpy(form, "%d\t%llu\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.LE);
         break;
      case 'f':
         strcpy(form, "%d\t%g\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.f);
         break;
      case 'd':
         strcpy(form, "%d\t%g\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.d);
         break;
      case 'v':
         strcpy(form, "%d\t%Lg\n");
         if (ff) {
            strcpy(form, "%d\t");
            strcat(form, format);
            strcat(form, "\n");
         }
         printf(form, i + lflag, x.de);
         break;
      }
      if (eflag)
         printf("%d\t0\n", i + lflag);
   }

   return 0;
}
