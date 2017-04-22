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

    $Id: _lspcheck.c,v 1.20 2016/12/22 10:53:07 fjst15124 Exp $

    Check order of LSP

       int lspcheck(lsp, ord)

       double   *lsp  : LSP
       int      ord   : order of LSP

       return   value : 0   -> normal
                        -1  -> ill condition

*****************************************************************/
#include<stdio.h>
#include<stdlib.h>

#if defined(WIN32)
#include "SPTK.h"
#else
#include <SPTK.h>
#endif

#define TH 100

int lspcheck(double *lsp, const int ord)
{
   int i;

   for (i = 1; i < ord; i++) {
      if (lsp[i] <= lsp[i - 1])
         return (-1);
   }
   if ((lsp[0] <= 0.0) || (lsp[ord - 1] >= 0.5))
      return (-1);

   return (0);
}

/****************************************************************

    $Id: _lspcheck.c,v 1.20 2016/12/22 10:53:07 fjst15124 Exp $

    Rearrangement of LSP

       void lsparrange(lsp, ord, min)

       double    *lsp : LSP
       int        ord : order of LSP
       double     min : minimal distance between two consecutive LSPs

*****************************************************************/

void lsparrange(double *lsp, int ord, double min)
{
   int i, count, flag;
   double tmp;

   /* check distance between two consecutive LSPs */
   for (count = 0; count < TH; count++) {
      flag = 0;
      for (i = 1; i < ord; i++) {
         tmp = lsp[i] - lsp[i - 1];
         if (min > tmp) {
            lsp[i - 1] -= (min - tmp) / 2;
            lsp[i] += (min - tmp) / 2;
            flag = 1;
         }
      }
      if (lsp[0] < 0.0) {
         lsp[0] = min;
         flag = 1;
      }
      if (lsp[ord - 1] > 0.5) {
         lsp[ord - 1] = 0.5 - min;
         flag = 1;
      }

      if (!flag)
         break;
   }

   return;
}
