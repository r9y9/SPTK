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
/*                1996-2016  Nagoya Institute of Technology          */
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

/********************************************************************
    $Id: fileio.c,v 1.22 2016/12/22 10:53:15 fjst15124 Exp $

    File I/O Functions

    int freada  (p, bl, fp)
    int fwritex (ptr, size, nitems, fp)
    int freadx  (ptr, size, nitems, fp)
    int fwritef (ptr, size, nitems, fp)
    int freadf  (ptr, size, nitems, fp)

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include <fcntl.h>
#include <io.h>
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

#define LINEBUFSIZE 256

/* freada: read ascii */
int freada(double *p, const int bl, FILE * fp)
{
   int c;
   char buf[LINEBUFSIZE];

#if defined(WIN32)
   _setmode(_fileno(fp), _O_TEXT);
#endif

   c = 0;
   while (c < bl) {
      if (fgets(buf, LINEBUFSIZE, fp) == NULL)
         break;
      p[c] = atof(buf);
      c++;
   }
   return (c);
}

/* fritex: wrapper function for fwrite */
int fwritex(void *ptr, const size_t size, const int nitems, FILE * fp)
{
#if defined(WIN32)
   _setmode(_fileno(fp), _O_BINARY);
#endif
   return (fwrite(ptr, size, nitems, fp));
}

/* freadx: wrapper function for fread */
int freadx(void *ptr, const size_t size, const int nitems, FILE * fp)
{
#if defined(WIN32)
   _setmode(_fileno(fp), _O_BINARY);
#endif
   return (fread(ptr, size, nitems, fp));
}

/* --------------- double I/O compile --------------- */
#ifdef DOUBLE
/* fwritef : write double type data */
int fwritef(double *ptr, const size_t size, const int nitems, FILE * fp)
{
   return (fwritex(ptr, size, nitems, fp));
}

/* freadf : read double type data */
int freadf(double *ptr, const size_t size, const int nitems, FILE * fp)
{
   return (freadx(ptr, size, nitems, fp));
}

#else                           /* DOUBLE */
/* --------------- float I/O compile --------------- */

static float *f;
static int items;

/* fwritef : convert double type data to float type and write */
int fwritef(double *ptr, const size_t size, const int nitems, FILE * fp)
{
   int i;
   if (items < nitems) {
      if (f != NULL)
         free(f);
      items = nitems;
      f = fgetmem(items);
   }
   for (i = 0; i < nitems; i++)
      f[i] = ptr[i];

#if defined(WIN32)
   _setmode(_fileno(fp), _O_BINARY);
#endif

   return fwrite(f, sizeof(float), nitems, fp);
}

/* freadf : read float type data and convert to double type */
int freadf(double *ptr, const size_t size, const int nitems, FILE * fp)
{
   int i, n;
   if (items < nitems) {
      if (f != NULL)
         free(f);
      items = nitems;
      f = fgetmem(items);
   }
#if defined(WIN32)
   _setmode(_fileno(fp), _O_BINARY);
#endif

   n = fread(f, sizeof(float), nitems, fp);
   for (i = 0; i < n; i++)
      ptr[i] = f[i];

   return n;
}
#endif                          /* DOUBLE */

void SPTK_byte_swap(void *p, size_t size, size_t num)
{
   char *q, tmp;
   size_t i, j;

   q = (char *) p;

   for (i = 0; i < num; i++) {
      for (j = 0; j < (size / 2); j++) {
         tmp = *(q + j);
         *(q + j) = *(q + (size - 1 - j));
         *(q + (size - 1 - j)) = tmp;
      }
      q += size;
   }
}

int fwrite_little_endian(void *buf, const size_t size,
                         const size_t n, FILE * fp)
{
#ifdef WORDS_BIGENDIAN
   SPTK_byte_swap(buf, size, n);
#endif
   return fwrite(buf, size, n, fp);
}
