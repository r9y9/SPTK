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
*    Simple APIs for playing audio data in windows                      *
*                                                                       *
*                                            2007.10  Jong-Jin Kim      *
*                                                                       *
*    notice: just for PCM, MONO                                         *
*                                                                       *
*************************************************************************/

/* $Id: winplay.h,v 1.7 2013/12/16 09:01:54 mataki Exp $ */

#ifndef __WINPLAY_H__
#define __WINPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WIN32AUDIO_NO_ERROR 0
#define WIN32AUDIO_FAILED -1

/*
   open audio device

   @param sr  [in] sampling frequency(Hz)
   @param sb  [in] sampling bits(bits)
   @return int
            WIN32AUDIO_FAILED    failed to open device
            WIN32AUDIO_NO_ERROR  ok
*/
   int win32_audio_open(int sr, int sb);

/*
   play audio data
   
   @param [in] buffer audio data buffer pointer to play
   @param [in] buffer_len number of audio samples to play
   @return int
            WIN32AUDIO_FAILED    failed to open device
            WIN32AUDIO_NO_ERROR  ok
*/
   int win32_audio_play(short *buffer, int buffer_len);

/*
   set volume
   
   @param [in] vol input volume value (0 ~ 65535)
   @return none
*/
   void win32_audio_set_volume(int vol);

/*
   close audio device
   
   @param none
   @return none
*/
   void win32_audio_close(void);

#ifdef __cplusplus
}
#endif
#endif                          /* __WINPLAY_H__ */
