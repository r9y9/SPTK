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

/* $Id: da.h,v 1.17 2013/12/16 09:01:54 mataki Exp $ */

#include <stdio.h>
#include <fcntl.h>
#ifndef WIN32
#include <sys/ioctl.h>
#endif

#if defined(LINUX) || defined(FreeBSD)
#  include <sys/soundcard.h>
#  define AUDIO_DEV "/dev/dsp"
#  define MIXER_DEV "/dev/mixer"
#  define MAXAMPGAIN 100
#  define AVAILABLE_FREQ "8,11.025,22.05,44.1"
#  define DEFAULT_FREQ 11
#endif                          /* LINUX or FreeBSD */

#ifdef SUNOS
#  define SPARC
#  include <sun/audioio.h>
#endif                          /* SUNOS */

#ifdef SOLARIS
#  define SPARC
#  include <sys/audioio.h>
#endif                          /* SOLARIS */

#ifdef SPARC
#  define AUDIO_DEV "/dev/audio"
#  define AUDIO_CTLDEV "/dev/audioctl"
#  define MAXAMPGAIN 255
#  define AVAILABLE_FREQ "8,11.025,16,22.05,32,44.1,48"
#  define DEFAULT_FREQ 16
#endif                          /* SPARC */

#ifdef WIN32
#  define AVAILABLE_FREQ  "8,11.025,22.05,44.1"
#  define MAXAMPGAIN      65535
#  define DEFAULT_FREQ    16
#endif

#define U_LAW 1
#define A_LAW 2
#define LINEAR 3

typedef struct _MENU {
   int value;
   unsigned int sample;
   unsigned int precision;
   unsigned int encoding;
} MENU;

static MENU data_type[] = {
   {0, 0, 0, 0},
   {1, 8000, 8, U_LAW},
   {2, 8000, 8, A_LAW},
   {3, 8000, 16, LINEAR},
   {4, 9600, 16, LINEAR},
   {5, 11025, 16, LINEAR},
   {6, 16000, 16, LINEAR},
   {7, 18900, 16, LINEAR},
   {8, 22050, 16, LINEAR},
   {9, 32000, 16, LINEAR},
   {10, 37800, 16, LINEAR},
   {11, 44100, 16, LINEAR},
   {12, 48000, 16, LINEAR}
};

#define _8000_8BIT_ULAW     1
#define _8000_8BIT_ALAW     2
#define _8000_16BIT_LINEAR  3
#define _9600_16BIT_LINEAR  4
#define _11025_16BIT_LINEAR 5
#define _16000_16BIT_LINEAR 6
#define _18900_16BIT_LINEAR 7
#define _22050_16BIT_LINEAR 8
#define _32000_16BIT_LINEAR 9
#define _37800_16BIT_LINEAR 10
#define _44100_16BIT_LINEAR 11
#define _48000_16BIT_LINEAR 12

int ACFD;
int ADFD;
FILE *adfp;


#define SPEAKER   (0x01)
#define HEADPHONE (0x02)
#define LINE_OUT  (0x04)
#define MUTE      (0x08)
