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

/****************************************************************

    $Id: _mlpg.c,v 1.9 2016/12/22 10:53:08 fjst15124 Exp $

    MLPG related functsions

*****************************************************************/

#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#ifndef HAVE_STRRCHR
#define strrchr rindex
#endif
#endif

#include <stdlib.h>
#include <ctype.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif


int str2darray(char *c, double **x)
{
   int i, size, sp;
   char *p, *buf;

   while (isspace(*c))
      c++;
   if (*c == '\0') {
      *x = NULL;
      return (0);
   }

   size = 1;
   sp = 0;
   for (p = c; *p != '\0'; p++) {
      if (!isspace(*p)) {
         if (sp == 1) {
            size++;
            sp = 0;
         }
      } else
         sp = 1;
   }
   buf = getmem(strlen(c), sizeof(*buf));
   *x = dgetmem(size);
   for (i = 0; i < size; i++)
      (*x)[i] = strtod(c, &c);
   return (size);
}

int isfloat(char *c)
{
   int isnum = 0, wfe = 1;
   int i = 0;

   if (strlen(c) == 0)
      return (0);

   if ((c[i] == '+') || (c[i] == '-'))
      i++;
   while ((c[i] >= '0') && (c[i] <= '9')) {
      isnum = 1;
      i++;
   }
   if (c[i] == '.') {
      i++;
      while ((c[i] >= '0') && (c[i] <= '9')) {
         isnum = 1;
         i++;
      }
   }
   if ((c[i] == 'e') || (c[i] == 'E')) {
      wfe = 0;
      i++;
      if ((c[i] == '+') || (c[i] == '-'))
         i++;
      while ((c[i] >= '0') && (c[i] <= '9')) {
         wfe = 1;
         i++;
      }
   }
   if ((c[i] == 'f') || (c[i] == 'F') || (c[i] == 'l') || (c[i] == 'L'))
      i++;

   if ((c[i] == '\0') && isnum && wfe)
      return (1);
   else
      return (0);
}
