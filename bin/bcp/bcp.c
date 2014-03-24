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
*    Block Copy                                                         *
*                                                                       *
*                                       1988.7  T.Kobayashi             *
*                                       1996.5  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               bcp [options] [infile] > stdout                         *
*       options:                                                        *
*               -l l     :  number of items contained 1 block   [512]   *
*               -L L     :  number of destination block size    [N/A]   *
*               -n n     :  order of items contained 1 block    [l-1]   *
*               -N N     :  order of destination block size     [N/A]   *
*               -s s     :  start number                        [0]     *
*               -S S     :  start number in destination block   [0]     *
*               -e e     :  end number                          [EOF]   *
*               -f f     :  fill into empty block               [0.0]   *
*               +type    :  data type                           [f]     *
*                           c (char)           C  (unsigned char)       *
*                           s (short)          S  (unsigned short)      *
*                           i (int)            I  (unsigned int)        *
*                           i3 (int, 3byte)    I3 (unsigned int, 3byte) *
*                           l (long)           L  (unsigned long)       *
*                           le (long long)     LE (unsigned long long)  *
*                           f (float)          d  (double)              *
*                           de (long double)   a  (ascii)               *
*       infile:                                                         *   
*               data sequence                                   [stdin] *
*       stdout:                                                         *
*               copied data sequence                                    *
*       note:                                                           *
*               When both (-L and -N) or (-l and -n) are specified,     *
*               latter argument is adopted.                             *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: bcp.c,v 1.28 2013/12/16 09:01:53 mataki Exp $";


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

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define START 0
#define END -1
#define DSTART 0
#define ITEM 512
#define DITEM 0
#define FILL 0.0


/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - block copy\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -l l  : number of items contained 1 block      [%d]\n",
           ITEM);
   fprintf(stderr,
           "       -L L  : number of destination block size       [N/A]\n");
   fprintf(stderr,
           "       -n n  : order of items contained 1 block       [l-1]\n");
   fprintf(stderr,
           "       -N N  : order of destination block size        [N/A]\n");
   fprintf(stderr,
           "       -s s  : start number                           [%d]\n",
           START);
   fprintf(stderr,
           "       -S S  : start number in destination block      [%d]\n",
           DSTART);
   fprintf(stderr,
           "       -e e  : end number                             [EOF]\n");
   fprintf(stderr,
           "       -f f  : fill into empty block                  [%g]\n",
           FILL);
   fprintf(stderr,
           "       +type : data type                              [f]\n");
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
   fprintf(stderr, "                a (ascii)\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       data sequence                                  [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       copied data sequence\n");
   fprintf(stderr, "  note:\n");
   fprintf(stderr,
           "       When both (-L and -N) or (-l and -n) are specified,\n");
   fprintf(stderr, "       latter argument is adopted.\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

int sno = START, eno = END, dsno = DSTART, size = sizeof(float), nitems =
    ITEM, dnitems = DITEM;
long double fl = FILL;
char type = 'f';

union typex {
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
} fillx;

int main(int argc, char **argv)
{
   FILE *fp = stdin;
   char *s, c;

   void bcp(FILE * fp);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 'n':
            nitems = atoi(*++argv) + 1;
            --argc;
            break;
         case 'N':
            dnitems = atoi(*++argv) + 1;
            --argc;
            break;
         case 'b':
            nitems = atoi(*++argv);
            --argc;
            break;
         case 'l':
            nitems = atoi(*++argv);
            --argc;
            break;
         case 'B':
            dnitems = atoi(*++argv);
            --argc;
            break;
         case 'L':
            dnitems = atoi(*++argv);
            --argc;
            break;
         case 's':
            sno = atoi(*++argv);
            --argc;
            break;
         case 'S':
            dsno = atoi(*++argv);
            --argc;
            break;
         case 'e':
            eno = atoi(*++argv) + 1;
            --argc;
            break;
         case 'f':
            fl = atof(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (*s == '+') {
         type = *++s;
         switch (type) {
         case 'a':
            size = 0;
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
      } else if (size == 0)
         fp = getfp(*argv, "rt");
      else
         fp = getfp(*argv, "rb");

   if (eno < 0)
      eno = nitems;
   if (sno < 0 || sno >= nitems || sno > eno || eno > nitems || dsno < 0)
      return (1);

   if (dnitems == 0)
      dnitems = eno - sno + dsno;

   bcp(fp);

   return (0);
}

void bcp(FILE * fp)
{
   char *buf, *lz = NULL, *fz = NULL;
   int ibytes, obytes, offset, nlz, nfz;
   void acopy(FILE * fp);
   void filln(char *ptr, int size, int nitem);

   switch (type) {
   case 'c':
      fillx.c = (char) fl;
      break;
   case 's':
      fillx.s = (short) fl;
      break;
   case 'i':
      fillx.i = (int) fl;
      break;
   case 't':
      fillx.i3 = (int) fl;
      break;
   case 'l':
      fillx.l = (long) fl;
      break;
   case 'u':
      fillx.le = (long long) fl;
      break;
   case 'C':
      fillx.c = (unsigned char) fl;
      break;
   case 'S':
      fillx.s = (unsigned short) fl;
      break;
   case 'I':
      fillx.i = (unsigned int) fl;
      break;
   case 'T':
      fillx.I3 = (unsigned int) fl;
      break;
   case 'L':
      fillx.l = (unsigned long) fl;
      break;
   case 'U':
      fillx.le = (unsigned long long) fl;
      break;
   case 'f':
      fillx.f = (float) fl;
      break;
   case 'd':
      fillx.d = (double) fl;
      break;
   case 'v':
      fillx.de = (long double) fl;
      break;
   case 'a':
      break;
   }

   if (size == 0) {
      acopy(fp);
      return;
   }

   ibytes = size * nitems;
   offset = size * sno;
   obytes = size * (eno - sno);
   nlz = size * dsno;
   nfz = ((nfz = size * dnitems - nlz - obytes) < 0) ? 0 : nfz;

   if ((buf = (char *) dgetmem(ibytes + nlz + nfz)) == NULL)
      return;

   if (nlz) {
      lz = buf + ibytes;
      filln(lz, size, nlz);
   }

   if (nfz) {
      fz = buf + ibytes + nlz;
      filln(fz, size, nfz);
   }

   while (freadx(buf, sizeof(*buf), ibytes, fp) == ibytes) {
      if (nlz)
         fwritex(lz, sizeof(*lz), nlz, stdout);
      fwritex(buf + offset, sizeof(*buf), obytes, stdout);

      if (nfz)
         fwritex(fz, sizeof(*fz), nfz, stdout);
   }
}

void acopy(FILE * fp)
{
   char s[512];
   int n, dn;
   int getstr(FILE * fp, char *s);

   for (dn = 0; !feof(fp);) {
      for (n = 0; n < sno; n++)
         if (getstr(fp, s) == 0)
            break;

      for (; n < eno; n++) {
         if (getstr(fp, s) == 0)
            break;
         if (dn++)
            putchar(' ');
         printf("%s", s);
         if (dn == dnitems) {
            putchar('\n');
            dn = 0;
         }
      }
      for (; n < nitems; n++)
         if (getstr(fp, s) == 0)
            break;
   }
}

int getstr(FILE * fp, char *s)
{
   int c;

   while ((c = getc(fp)) == ' ' || c == 't' || c == '\n' || c == ',');

   if (c == EOF)
      return (0);

   for (*s++ = c;
        (c = getc(fp)) != EOF && c != ' ' && c != '\t' && c != '\n'
        && c != ',';)
      *s++ = c;

   *s = '\0';
   return (1);
}

void filln(char *ptr, int size, int nitem)
{
   int i;
   char *c;

   nitem = nitem / size;
   while (nitem--) {
      c = &fillx.c;
      for (i = 1; i <= size; i++) {
         *ptr++ = *c++;
      }
   }
}
