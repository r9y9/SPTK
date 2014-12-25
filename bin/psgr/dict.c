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

/* $Id: dict.c,v 1.12 2014/12/11 08:30:48 uratec Exp $ */

#include <stdio.h>

void dict(void)
{
   puts("/BD{bind def}bind def");
   puts("/MT{moveto}BD");
   puts("/LT{lineto}BD");
   puts("/RMT{rmoveto}BD");
   puts("/RLT{rlineto}BD");
   puts("/ST{stroke}BD");

   puts("/FnC{/Courier}BD");
   puts("/FnCO{/Courier-Oblique}BD");
   puts("/FnCB{/Courier-Bold}BD");
   puts("/FnCBO{/Courier-BoldOblique}BD");
   puts("/FnS{/Symbol}BD");
   puts("/SF{");                /*  select font  */
   puts("  exch findfont exch");
   puts("  dup type /arraytype eq{makefont}{scalefont}ifelse");
   puts("  setfont");
   puts("}BD");

   puts("/MS{MT show}BD");
   puts("/RMS{RMT show}BD");
   puts("/GS{gsave}BD");
   puts("/GR{grestore}BD");
   puts("/NP{newpath}BD");

   /*  set rect path  */
   puts("/RP{");
   puts("dup type dup /integertype eq exch /realtype eq or {");
   puts("    4 -2 roll MT");
   puts("    dup 0 exch RLT exch 0 RLT");
   puts("    neg 0 exch RLT closepath");
   puts("  }{");
   puts("    dup length 4 sub 0 exch 4 exch");
   puts("    {");
   puts("      1 index exch 4 getinterval");
   puts("      aload pop");
   puts("      RP");
   puts("    }for");
   puts("    pop");
   puts("  }ifelse");
   puts("}BD");
   puts("/RS{GS NP RP stroke GR}BD");
   puts("/RF{GS NP RP fill GR}BD");
   puts("/RC{NP RP clip NP}BD");

   puts("/TR{translate}BD");
   puts("/SL{setlinewidth}BD");
   puts("/SG{setgray}BD");

   /*  fill pattern  */
   /* puts("/PM{32 32 1 [32 0 0 32 0 0]}BD");
      puts("/P2{<0102040801020408>}def");
      puts("/P3{<8040201008040201>}def");
      puts("/P4{<8888888888888888>}def");
      puts("/P5{<8142241812484281>}def");
      puts("/P6{<FF000000FF000000>}def");
      puts("/P7{<FF888888FF888888>}def");
      puts("/IM{image}BD");
    */
}
