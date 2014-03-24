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

/****************************************************************
*                                                               *
*    XY-Plotter simulator for X Window System                   *
*                                                               *
*    1992.03 T. KANNO                                           *
*                                                               *
*       usage:                                                  *
*               xgr [ options ] [ infile ] > stdout             *
*       options:                                                *
*               -s  s  : shrink                    [3.38667]    *
*               -l     : landscape                 [FALSE]      *
*               -rv    : reverse mode              [FALSE]      *
*               -m     : monochrome display mode   [FALSE]      *
*               -bg bg : background color          [white]      *
*               -hl hl : highlight color           [blue]       *
*               -bd bd : border color              [blue]       *
*               -ms ms : mouse color               [red]        *
*               -g  g  : geometry                  [NULL]       *
*               -d  d  : display                   [NULL]       *
*               -t  t  : window title              [xgr]        *
*       infile:                                                 *
*               plotter commands                                *
*                                                               *
****************************************************************/

static char *rcs_id = "$Id: xgr.c,v 1.22 2013/12/16 09:02:06 mataki Exp $";


/*  Standard C Libraries */
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

#include "xgr.h"

/* Defalut Value */
#define LANDSCAPE FA
#define REVERSE  FA
#define C_FLG  FA
#define MONO_FLG  FA
#define FNO  1
#define FORECOLOR "black"
#define BACKCOLOR "white"
#define HIGHCOLOR "blue"
#define BRDRCOLOR "blue"
#define MOUSCOLOR "red"
#define GEOMETRY NULL
#define DISPLAY  NULL

typedef enum _Boolean { FA, TR } Boolean;
char *BOOL[] = { "FALSE", "TRUE" };

/* Command Name */
char *cmnd;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - XY-plotter simulator for X-window system\n\n", cmnd);
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -s  s  : shrink                  [%g]\n",
           DEFAULT_SHRINK);
   fprintf(stderr, "       -l     : landscape               [%s]\n",
           BOOL[LANDSCAPE]);
   fprintf(stderr, "       -rv    : reverse mode            [%s]\n",
           BOOL[REVERSE]);
   fprintf(stderr, "       -m     : monochrome display mode [%s]\n",
           BOOL[MONO_FLG]);
   fprintf(stderr, "       -bg bg : background color        [%s]\n", BACKCOLOR);
   fprintf(stderr, "       -hl hl : highlight color         [%s]\n", HIGHCOLOR);
   fprintf(stderr, "       -bd bd : border color            [%s]\n", BRDRCOLOR);
   fprintf(stderr, "       -ms ms : mouse color             [%s]\n", MOUSCOLOR);
   fprintf(stderr, "       -g  g  : geometry                [NULL]\n");
   fprintf(stderr, "       -d  d  : display                 [NULL]\n");
   fprintf(stderr, "       -t  t  : window title            [%s]\n", cmnd);
   fprintf(stderr, "       -h     : print this message \n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       plotter commands                 [stdin]\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

float shrink = DEFAULT_SHRINK;
int landscape = LANDSCAPE;
int reverse = REVERSE, c_flg = C_FLG, mono_flg = MONO_FLG;
int fno = FNO;
char *fore_color = FORECOLOR, *back_color = BACKCOLOR, *high_color =
    HIGHCOLOR, *brdr_color = BRDRCOLOR, *mous_color = MOUSCOLOR;
char *geometry = GEOMETRY, *dpy = DISPLAY, *windowtitle;

int main(int argc, char *argv[])
{
   char c;

   cmnd = windowtitle = *argv;
   if (strrchr(cmnd, '/'))
      cmnd = windowtitle = (char *) (strrchr(cmnd, '/') + 1);

   while (--argc) {
      if ((strcmp(*++argv, "-s") == 0) && argc > 1) {
         shrink = atof(*++argv);
         if (shrink <= 0)
            usage(0);
         argc--;
      } else if (strcmp(*argv, "-landscape") == 0 ||
                 strcmp(*argv, "-land") == 0 || strcmp(*argv, "-l") == 0) {
         landscape = 1 - landscape;
      } else if (strcmp(*argv, "-reverse") == 0 || strcmp(*argv, "-rv") == 0) {
         reverse = 1 - reverse;
      } else if (strcmp(*argv, "-color") == 0 || strcmp(*argv, "-c") == 0) {
         c_flg = 1 - c_flg;
      } else if (strcmp(*argv, "-mono") == 0 || strcmp(*argv, "-m") == 0) {
         mono_flg = 1 - mono_flg;
      } else if (strcmp(*argv, "-fno") == 0 && argc > 1) {
         fno = atoi(*++argv);
         argc--;
      } else if (strcmp(*argv, "-bold") == 0 || strcmp(*argv, "-b") == 0) {
         fno += 4;
      } else if (strcmp(*argv, "-fg") == 0 && argc > 1) {
         fore_color = *++argv;
         argc--;
      } else if (strcmp(*argv, "-bg") == 0 && argc > 1) {
         back_color = *++argv;
         argc--;
      } else if (strcmp(*argv, "-hl") == 0 && argc > 1) {
         high_color = *++argv;
         argc--;
      } else if (strcmp(*argv, "-bd") == 0 && argc > 1) {
         brdr_color = *++argv;
         argc--;
      } else if (strcmp(*argv, "-ms") == 0 && argc > 1) {
         mous_color = *++argv;
         argc--;
      } else if ((strcmp(*argv, "-geometry") == 0 ||
                  strcmp(*argv, "-g") == 0) && argc > 1) {
         geometry = *++argv;
         argc--;
      } else if ((strcmp(*argv, "-display") == 0 ||
                  strcmp(*argv, "-d") == 0) && argc > 1) {
         dpy = *++argv;
         argc--;
      } else if ((strcmp(*argv, "-title") == 0 ||
                  strcmp(*argv, "-t") == 0) && argc > 1) {
         windowtitle = *++argv;
         argc--;
      } else if (strcmp(*argv, "-h") == 0 ||
                 strcmp(*argv, "--") == 0 || strcmp(*argv, "-?") == 0) {
         usage(0);
      } else
         usage(1);
   }
   ungetc(c = getchar(), stdin);
   if (c == EOF)
      return (-1);
   else if (c != '=') {
      fprintf(stderr, "%s : Unexpected data format!\n", cmnd);
      return (-1);
   }
   open_display(dpy);
   init_window(argc, argv);
   init_pixmap();
   plot();
   main_loop();
   close_window();

   return (0);
}
