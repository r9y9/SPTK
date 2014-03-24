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
*  Swap Bytes                                                           *
*                                                                       *
*                                   1996    T.Kobayashi                 *
*                                   1997.3  G.Hirabayashi               *
*                                   2010.3  A.Tamamori                  *
*                                                                       *
*     usage:                                                            *
*             swab [ options ] [ infile ] > stdout                      *
*     options:                                                          *
*             -S S     :  start address                [0]              *
*             -s s     :  start offset number          [0]              *
*             -E E     :  end address                  [EOF]            *
*             -e e     :  end offset number            [0]              *
*             +type    :  input and output data type   [s]              *
*                           c (char)           C  (unsigned char)       *
*                           s (short)          S  (unsigned short)      *
*                           i (int)            I  (unsigned int)        *
*                           i3 (int, 3byte)    I3 (unsigned int, 3byte) *
*                           l (long)           L  (unsigned long)       *
*                           le (long long)     LE (unsigned long long)  *
*                           f (float)          d  (double)              *
*                           de (long double)                            *
*     infile:                                                           *
*             data sequence                            [stdin]          * 
*     stdout:                                                           *
*             swapped data sequence                                     *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: swab.c,v 1.29 2013/12/16 09:02:04 mataki Exp $";


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


#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Command Name  */
char *cmnd;


/*  Default Values  */
#define START 0
#define SNO   0
#define END   0x7fffffff
#define ENO   0x7fffffff
#define BUFSIZE 12
#define SIGNED_INT3 FA
#define UNSIGNED_INT3 FA

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - Swap Bytes\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -S S   : start address                [%d]\n",
           START);
   fprintf(stderr, "       -s s   : start offset number          [%d]\n", SNO);
   fprintf(stderr, "       -E E   : end address                  [EOF]\n");
   fprintf(stderr, "       -e e   : end offset number            [0]\n");
   fprintf(stderr, "       +type  : input and output data format [s]\n");
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
   fprintf(stderr, "       -h     : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       data sequence                [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       swapped data sequence\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

static long start = START, _end = END, sno = SNO, eno = ENO;

int main(int argc, char *argv[])
{
   FILE *fp = stdin;
   char *s;
   int c;
   size_t iosize = sizeof(short);
   void conv(FILE * fp, size_t iosize);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 'S':
            start = atol(*++argv);
            --argc;
            break;
         case 's':
            sno = atol(*++argv);
            --argc;
            break;
         case 'E':
            _end = atol(*++argv);
            --argc;
            break;
         case 'e':
            eno = atol(*++argv);
            --argc;
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
         case 's':
            iosize = sizeof(short);
            break;
         case 'i':
            if (*(s + 1) == '3')
               iosize = 3;
            else
               iosize = sizeof(int);
            break;
         case 'l':
            if (*(s + 1) == 'e')
               iosize = sizeof(long long);
            else
               iosize = sizeof(long);
            break;
         case 'S':
            iosize = sizeof(unsigned short);
            break;
         case 'I':
            if (*(s + 1) == '3')
               iosize = 3;
            else
               iosize = sizeof(unsigned int);
            break;
         case 'L':
            if (*(s + 1) == 'E')
               iosize = sizeof(unsigned long long);
            else
               iosize = sizeof(unsigned long);
            break;
         case 'f':
            iosize = sizeof(float);
            break;
         case 'd':
            if (*(s + 1) == 'e')
               iosize = sizeof(long double);
            else
               iosize = sizeof(double);
            break;
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fp = getfp(*argv, "rb");

   conv(fp, iosize);

   return (0);
}

void conv(FILE * fp, size_t iosize)
{
   long adrs, n;
   int i;
   char ibuf[BUFSIZE], obuf[BUFSIZE];
   int ffseek(FILE * fp, long off);

   if (ffseek(fp, adrs = start + iosize * sno))
      return;

   for (n = sno; (adrs <= _end) && (n <= eno); adrs += iosize, ++n) {
      freadx(ibuf, iosize, 1, fp);
      if (feof(fp))
         break;
      for (i = 0; i < (int) iosize; ++i)
         obuf[i] = ibuf[iosize - 1 - i];
      fwritex(obuf, iosize, 1, stdout);
   }

   return;
}

int ffseek(FILE * fp, long off)
{
   int n;

   if (fp != stdin)
      fseek(fp, off, 0);
   else {
      for (n = off; n; --n)
         if (getc(fp) == EOF)
            return (1);
   }
   return (0);
}
