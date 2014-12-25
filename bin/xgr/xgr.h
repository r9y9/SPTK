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
 * $Id: xgr.h,v 1.14 2014/12/11 08:30:52 uratec Exp $
 *  Headder for xgr
 */


/*  for SUN  */

#define DEFAULT_SHRINK   (254.0/75.0)
#define X0      558             /*  max -> 530   */
#define Y0      1               /*  max -> 23   */
#define X0_LAND 300

#if     NeXT
#define DEFAULT_SHRINK   3.6
#define X0      561
#define Y0      1

#elif   DGAV
#define DEFAULT_SHRINK   3.0    /*  max -> 2.9  */
#define X0      610             /*  max -> 590  */
#define Y0      1
#endif

#define XLENG   1995            /*  A4 Width  [x0.1 mm](1950,max=2095)  */
#define YLENG   2870            /*  A4 Height [x0.1 mm](2850,max=2970)  */
#define XLENG_LAND   2870
#define YLENG_LAND   1995

typedef struct cord {
   int x;
   int y;
} Cord;

void open_display(char *display_name);
void init_window(int argc, char *argv[]);
void init_pixmap(void);
void close_window(void);
void main_loop(void);
void realize(void);
void realize_part(int src_x, int src_y, int width, int height, int dest_x,
                  int dest_y);
void set_all(void);
void beep(void);
void get_window_size(void);
unsigned long get_color_pix(char *color_name);
void plot(void);
