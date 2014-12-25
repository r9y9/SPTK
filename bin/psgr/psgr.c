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

/********************************************************
*                                                       *
*   psgr:  xy-plotter simulator for EPSF                *
*                                                       *
*      Ver. 0.95  '92.3 T.Kanno                         *
*      Ver. 0.96  '92.8                                 *
*      Ver. 0.97  '92.10                                *
*      Ver. 0.98  '93.2                                 *
*      Ver. 0.99  '93.8                                 *
********************************************************/

static char *rcs_id = "$Id: psgr.c,v 1.25 2014/12/11 08:30:48 uratec Exp $";


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

#include "psgr.h"


char *BOOL[] = { "FALSE", "TRUE" };


#define MaxPaperTypes 14        /*  Paper Media  */

struct page_media paper[] = {
   {"FALSE", 9999, 9999},
   {"Letter", 612, 792},
   {"A0", 2378, 3362},
   {"A1", 1682, 2378},
   {"A2", 1190, 1682},
   {"A3", 842, 1190},
   {"A4", 842, 842},
/* {"A4",      595,  842}, */
   {"A5", 420, 595},
   {"B0", 2917, 4124},
   {"B1", 2063, 2917},
   {"B2", 1459, 2063},
   {"B3", 1032, 1459},
   {"B4", 729, 1032},
   {"B5", 516, 729},
};

char *orientations[] = {        /*  Orientation  */
   "Portrait",
   "Landscape",
};


/* Default Values */
#define MEDIA       "FALSE"
#define ORIENTATION "Portrait"
#define PSMODE      FA
#define PAPERNUM    2
#define XLENG       595
#define YLENG       842
#define LANDSCAPE   FA
#define RESOLUTION  600
#define FONTNO      1
#define CLIPMODE    FA
#define NCOPY       1
#define XOFFSET     0
#define YOFFSET     0
#define SHRINK      1.0
#define SCALE 10


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - XY-plotter simulator for EPSF\n\n", progname);
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ infile ] > stdout\n", progname);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -t t  : title of figure      [NULL]\n");
   fprintf(stderr, "       -s s  : shrink               [%g]\n", SHRINK);
   fprintf(stderr, "       -c c  : number of copy       [%d]\n", NCOPY);
   fprintf(stderr, "       -x x  : x offset <mm>        [%d]\n", XOFFSET);
   fprintf(stderr, "       -y y  : y offset <mm>        [%d]\n", YOFFSET);
   fprintf(stderr, "       -p p  : paper                [%s]\n", MEDIA);
   fprintf(stderr,
           "               (Letter,A0,A1,A2,A3,A4,A5,B0,B1,B2,B3,B4,B5)\n");
   fprintf(stderr, "       -l    : landscape            [%s]\n",
           BOOL[LANDSCAPE]);
   fprintf(stderr, "       -r r  : resolution           [%d dpi]\n",
           RESOLUTION);
   fprintf(stderr, "       -b    : bold mode            [FALSE]\n");
   fprintf(stderr, "       -T T  : top    margin <mm>   [%d]\n", bbm.top);
   fprintf(stderr, "       -B B  : bottom margin <mm>   [%d]\n", bbm.bottom);
   fprintf(stderr, "       -L L  : left   margin <mm>   [%d]\n", bbm.left);
   fprintf(stderr, "       -R R  : right  margin <mm>   [%d]\n", bbm.right);
   fprintf(stderr, "       -P    : output PS            [%s]\n", BOOL[PSMODE]);
   fprintf(stderr, "       -h    : print this message \n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       plotter commands             [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       PostScript codes (EPSF)\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}

char *progname, *filename = NULL, *title = NULL;
char *media = MEDIA, *orientation = ORIENTATION;

int paper_num = PAPERNUM, xleng = XLENG, yleng = YLENG, resolution = RESOLUTION;
int font_no = FONTNO, psmode = PSMODE, landscape = LANDSCAPE, clip_mode =
    CLIPMODE;


int main(int argc, char *argv[])
{
   char *str, flg, c;
   FILE *fp = NULL;
   int i;
   int ncopy = NCOPY, xoffset = XOFFSET, yoffset = YOFFSET;
   float shrink = SHRINK;

   progname = *argv;
   if (strrchr(progname, '/'))
      progname = (char *) (strrchr(progname, '/') + 1);
   while (--argc) {
      if (*(str = *++argv) == '-') {
         flg = *++str;
         if ((flg != 'P' && flg != 'l' && flg != 'b')
             && *++str == '\0') {
            str = *++argv;
            argc--;
         }
         switch (flg) {
         case 'P':
            psmode = 1 - psmode;
            break;
         case 't':
            title = str;
            break;
         case 'c':
            ncopy = atoi(str);
            break;
         case 's':
            shrink = atof(str);
            break;
         case 'x':
            xoffset = atoi(str) * SCALE;
            break;
         case 'y':
            yoffset = atoi(str) * SCALE;
            break;
         case 'p':
            media = str;
            break;
         case 'l':
            landscape = 1 - landscape;
            break;
         case 'r':
            resolution = atoi(str);
            break;
         case 'T':
            bbm.top = atoi(str) * 10;
            break;
         case 'B':
            bbm.bottom = atoi(str) * 10;
            break;
         case 'L':
            bbm.left = atoi(str) * 10;
            break;
         case 'R':
            bbm.right = atoi(str) * 10;
            break;
         case 'b':
            font_no += 2;
            break;
         case 'h':
            usage(0);
            break;
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", progname, flg);
            usage(1);
            break;
         }
      } else
         filename = str;
   }
   for (i = 0; i < MaxPaperTypes; i++) {
      if (strcmp(media, paper[i].size) == 0) {
         paper_num = i;
         break;
      }
   }
   if (!landscape) {            /*  Portrait  */
      xleng = paper[paper_num].width;
      yleng = paper[paper_num].height;
   } else {                     /*  Landscape  */
      xleng = paper[paper_num].height;
      yleng = paper[paper_num].width;
   }
   xleng = xleng * (double) SCALE / shrink;
   yleng = yleng * (double) SCALE / shrink;

   orientation = orientations[landscape];

   if (filename != NULL) {
      fp = getfp(filename, "rt");
   } else {
      fp = tmpfile();
      while ((c = getchar()) != (char) EOF)
         fputc(c, fp);
      rewind(fp);
   }

   ungetc(flg = fgetc(fp), fp);
   if (flg == (char) EOF) {
      fprintf(stderr, "%s : Input file is empty!\n", progname);
      return (-1);
   } else if (flg != '=') {
      fprintf(stderr, "%s : Unexpected data format!\n", progname);
      return (-1);
   }

   epsf_setup(fp, shrink, xoffset, yoffset, bbm, ncopy);
   plot(fp);
   epsf_end();

   fclose(fp);
   return (0);
}
