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

/*
 * $Id: gcdata.h,v 1.15 2014/12/11 08:30:52 uratec Exp $
 *   Hedder for setup plotting parts
 */

#include "config.h"

#ifndef X11R3
#define SIZE 512                /* buffer size */
#else
#define SIZE 2                  /* buffer size for X11R3's BUG */
#endif
#define FCW  22
#define FCH  25
#define LFCH 50                 /* Large font height */

/*
 * Fonts name
 */
#define FSymbol 9
static char *f_name[] = {
   "-adobe-courier-medium-r-normal-*-*-100-*-*-m-*-iso8859-1",
   "-adobe-courier-medium-r-normal-*-*-120-*-*-m-*-iso8859-1",
   "-adobe-courier-medium-r-normal-*-*-140-*-*-m-*-iso8859-1",
   "-adobe-courier-medium-r-normal-*-*-160-*-*-m-*-iso8859-1",
   "-adobe-courier-bold-r-normal-*-*-100-*-*-m-*-iso8859-1",
   "-adobe-courier-bold-r-normal-*-*-120-*-*-m-*-iso8859-1",
   "-adobe-courier-bold-r-normal-*-*-140-*-*-m-*-iso8859-1",
   "-adobe-courier-bold-r-normal-*-*-160-*-*-m-*-iso8859-1",
   "-adobe-symbol-medium-r-normal-*-*-80-*-*-p-*-adobe-fontspecific",
   "-adobe-symbol-medium-r-normal-*-*-100-*-*-p-*-adobe-fontspecific",
   "-adobe-symbol-medium-r-normal-*-*-120-*-*-p-*-adobe-fontspecific",
   "-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific",
   "-adobe-symbol-medium-r-normal-*-*-160-*-*-p-*-adobe-fontspecific"
};
