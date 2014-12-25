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
*    File dump                                                          *
*                                                                       *
*                                       1988    T.Kobayashi             *
*               1996.5  K.Koishida                                      *
*                                                                       *
*       usage:                                                          * 
*               fd [options] [infile] > stdout                          *
*       options:                                                        *
*               -a a     :  address                             [0]     *
*               -n n     :  initial value for numbering         [0]     *
*               -m m     :  modulo for numbering                [EOF]   *
*               -ent     :  number of data in each line         [0]     *
*               +type    :  data type                           [c]     *
*                           c (char)           C  (unsigned char)       *
*                           s (short)          S  (unsigned short)      *
*                           i (int)            I  (unsigned int)        *
*                           i3 (int, 3byte)    I3 (unsigned int, 3byte) *
*                           l (long)           L  (unsigned long)       *
*                           le (long long)     LE (unsigned long long)  *
*                           f (float)          d  (double)              *
*                           de (long double)                            *
*               %form    :  print format(printf style)          [N/A]   *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: fd.c,v 1.31 2014/12/11 08:30:34 uratec Exp $";


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
#define MODULO 0x7fffffff
#define ENTRY 0
#define SIZE 128


/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - file dump\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -a a  : address                     [%d]\n", START);
   fprintf(stderr, "       -n n  : initial value for numbering [%d]\n", START);
   fprintf(stderr, "       -m m  : modulo for numbering        [EOF]\n");
   fprintf(stderr, "       -ent  : number of data in each line [%d]\n", ENTRY);
   fprintf(stderr, "       +type : data type                   [c]\n");
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
   fprintf(stderr, "       %%form : print format(printf style) [N/A]\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence                       [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       dumped data sequence\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}


long start = START, mod = MODULO;
int is_int = 0, entry = ENTRY, is_char = 1, ff = 0;
size_t size = sizeof(char);
char adrsfmt = 'd', format[SIZE], form[SIZE], type = 'c';

int main(int argc, char **argv)
{
   FILE *fp = stdin;
   char *s, c;
   void fdump(FILE * fp);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc)
      if (*(s = *++argv) == '-') {
         if (((c = *++s) > '0') && (c <= '9'))
            c = 'c';
         switch (c) {
         case 'a':
         case 'n':
            adrsfmt = c;
            start = atol(*++argv);
            --argc;
            break;
         case 'm':
            mod = atol(*++argv);
            --argc;
            break;
         case 'c':
            entry = atoi(s);
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
         c = type = *++s;
         switch (c) {
         case 'b':
            is_char = 0;
            break;
         case 'c':
            size = sizeof(char);
            break;
         case 's':
            size = sizeof(short);
            break;
         case 'i':
            if (*(s + 1) == '3') {
               size = 3;
               type = 't';
               (*argv)++;
            } else {
               size = sizeof(int);
            }
            break;
         case 'l':
            if (*(s + 1) == 'e') {
               size = sizeof(long long);
               type = 'u';
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
               type = 'T';
               (*argv)++;
            } else {
               size = sizeof(unsigned int);
            }
            break;
         case 'L':
            if (*(s + 1) == 'E') {
               size = sizeof(unsigned long long);
               type = 'U';
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
               type = 'v';
               (*argv)++;
            } else {
               size = sizeof(double);
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

   fdump(fp);

   return 0;
}

void fdump(FILE * fp)
{
   char s[18];
   long adrs, i, n;
   union {
      unsigned char b;
      short w;
      int i;
      int i3;
      long l;
      long long le;
      unsigned char C;
      unsigned short S;
      unsigned int I3;
      unsigned int I;
      unsigned long L;
      unsigned long long LE;
      float f;
      double d;
      long double de;
   } u;
   int y;

   if (ff)
      strcat(format, " ");

   if (entry == 0) {
      if (size == 8)
         entry = 2;
      else if (size > 1)
         entry = 5;
      else if (is_char)
         entry = 16;
      else
         entry = 10;
   }

   freadx(&u, size, 1, fp);

   while (!feof(fp))
      for (n = adrs = start; (n < mod) && !feof(fp); adrs += entry * size) {
         if (adrsfmt == 'a')
            printf("%06lx  ", adrs);
         else if (adrsfmt == 'n')
            printf("%7ld  ", n);

         for (i = 0; (i < entry) && !feof(fp) && (n < mod); i++, n++) {
            switch (type) {
            case 'c':
               if (isprint(u.b)) {
                  s[i] = u.b;
               } else {
                  s[i] = '.';
               }
               if (is_char)
                  printf("%02x ", u.b);
               else
                  printf("%5d", u.b);
               break;
            case 's':
               printf("%7d", u.w);
               break;
            case 't':
               y = u.i3 & 0x00FFFFFF;
               if (y >> 23 == 1)
                  y = y | 0xFF000000;
               if (ff)
                  printf(format, y);
               else
                  printf("%9d", y);
               break;
            case 'i':
               if (ff)
                  printf(format, u.i);
               else
                  printf("%12d", u.i);
               break;
            case 'l':
               if (ff)
                  printf(format, u.l);
               else
                  printf("%12ld", u.l);
               break;
            case 'u':
               if (ff)
                  printf(format, u.le);
               else
                  printf("%17lld", u.le);
               break;
            case 'C':
               if (ff)
                  printf(format, u.C);
               else
                  printf("%5u", u.C);
            case 'S':
               if (ff)
                  printf(format, u.S);
               else
                  printf("%7u", u.S);
               break;
            case 'T':
               y = u.I3 & 0x00FFFFFF;
               if (ff)
                  printf(format, y);
               else
                  printf("%9u", y);
               break;
            case 'I':
               if (ff)
                  printf(format, u.I);
               else
                  printf("%12u", u.I);
               break;
            case 'L':
               if (ff)
                  printf(format, u.L);
               else
                  printf("%12lu", u.L);
               break;
            case 'U':
               if (ff)
                  printf(format, u.LE);
               else
                  printf("%17llu", u.LE);
               break;
            case 'f':
               if (ff)
                  printf(format, u.f);
               else
                  printf("%14.6e", u.f);
               break;
            case 'd':
               if (ff)
                  printf(format, u.d);
               else
                  printf("%24.15e", u.d);
               break;
            case 'v':
               if (ff)
                  printf(format, u.de);
               else
                  printf("%24.15Le", u.de);
               break;
            default:
               break;
            }
            freadx(&u, size, 1, fp);
         }
         if (size == 1 && is_char) {
            s[i] = '\0';
            while (i++ < entry)
               printf("   ");
            printf("|%s|\n", s);
         } else
            printf("\n");
      }
}
