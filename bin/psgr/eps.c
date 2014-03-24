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

/********************************************************
* $Id: eps.c,v 1.20 2013/12/16 09:02:02 mataki Exp $   *
*            Setup Commands for EPSF                    *
********************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include "psgr.h"

#define plot_min(x, min, pmin) ((x<min) ? ((x>=pmin)?(x):(pmin)) : (min))
#define plot_max(x, max, pmax) ((x>max) ? ((x<=pmax)?(x):(pmax)) : (max))

float loffset;

static void epsf_init(int *xmin, int *ymin, int *xmax, int *ymax, int ncopy)
{
   char *user_name;
   char creation_date[64];
   long t;

   user_name = (char *) getenv("USER");
   t = time(NULL);
   strcpy(creation_date, asctime(localtime(&t)));

   if (psmode)
      printf("%%!PS-Adobe-2.0\n");
   else
      printf("%%!PS-Adobe-2.0 EPSF-1.2\n");

   if (title != NULL)
      printf("%%%%Title: %s\n", title);
   else if (filename != NULL)
      printf("%%%%Title: %s\n", filename);
   else
      printf("%%%%Title: Figure(s) by %s(fig/fdrw).\n", progname);

   /* printf("%%%%Creator: %s (Version %s, %s)\n", progname, Version, Date);
    */
   printf("%%%%For: %s\n", user_name);
   printf("%%%%CreationDate: %s", creation_date);

   if (!psmode)
      printf("%%%%BoundingBox: %d %d %d %d\n", *xmin, *ymin, *xmax, *ymax);
   printf("%%%%EndComments\n\n");

   printf("%%%%BeginProlog\n");
   dict();
   printf("%%%%EndProlog\n\n");

   printf("%%%%BeginSetup\n");
   printf("%%%%PaperSize: %s\n", media);
   printf("%%%%PageOrientation: %s\n", orientation);
   printf("%%%%Feature: *ManualFeed False\n");
   printf("%%%%Feature: *Resolution %d\n", resolution);
   if (ncopy > 1)
      printf("/#copies %d def\n", ncopy);
   printf("%%%%EndSetup\n\n");
   if (psmode)
      printf("%%%%Page: 1\n");

   return;
}

static void epsf_scale(float shrink, int xoffset, int yoffset)
{
   float unit_length;

   unit_length = shrink * PU_PT;

   if (!landscape) {
      printf("%d %d translate\n", norm(xoffset * PU_PT + SHIFT),
             norm(yoffset * PU_PT + SHIFT));
      printf("%f %f scale\n", unit_length, unit_length);
   } else {
      printf("%d %d translate\n", norm(xoffset * PU_PT + SHIFT),
             norm(yoffset * PU_PT + SHIFT));
      printf("%f %f scale\n", unit_length, unit_length);
      loffset = 4 * SHIFT / unit_length;
      printf("%d 0 translate\n", norm(yleng * PU_PT + loffset));
      /*  printf("%d 0 translate\n", norm((yleng+LAND_OFFSET)*PU_PT));
       */
      printf("90 rotate\n");
   }
   return;
}

static int getd(FILE * fp, int *x, int *y)
{
   static int c;

   while ((c = getc(fp)) == '\n' || c == ' ');

   if (isdigit(c)) {
      ungetc(c, fp);
      fscanf(fp, "%d %d", x, y);
      return (1);
   } else
      return (0);
}

static int getstrlength(FILE * fp)
{
   int n;
   int c;

   n = 0;
   while (isprint(c = getc(fp)) || (c & 0xff) > 0)
      n++;

   return (n);
}

static void bbox(FILE * fp, int *xmin, int *ymin, int *xmax, int *ymax,
                 float shrink, int xoffset, int yoffset, struct bbmargin bbm)
{
   char c;
   int n, x, y;
   int temp_xmin, temp_ymin, temp_xmax, temp_ymax;
   int temp_plot_xmin, temp_plot_ymin = 0, temp_plot_xmax, temp_plot_ymax;
   int plot_xmin, plot_ymin, plot_xmax, plot_ymax;
   double unit_length;
   int rotate = 0;
   int ch = 30, cw = 30;
   float mag = 0.875;           /*  7/8  */

   temp_xmin = temp_ymin = 9999;
   temp_xmax = temp_ymax = 0;
   plot_xmin = xleng;
   plot_ymin = yleng;
   plot_xmax = plot_ymax = 0;

   while ((c = getc(fp)) != (char) EOF) {
      switch (c) {
      case 'M':
         fscanf(fp, "%d %d", &x, &y);
         temp_xmin = (temp_xmin < x ? temp_xmin : x);
         temp_ymin = (temp_ymin < y ? temp_ymin : y);
         temp_xmax = (temp_xmax > x ? temp_xmax : x);
         temp_ymax = (temp_ymax > y ? temp_ymax : y);
         break;
      case 'D':
         while (getd(fp, &x, &y)) {
            temp_xmin = (temp_xmin < x ? temp_xmin : x);
            temp_ymin = (temp_ymin < y ? temp_ymin : y);
            temp_xmax = (temp_xmax > x ? temp_xmax : x);
            temp_ymax = (temp_ymax > y ? temp_ymax : y);
         }
         break;
      case '%':
         fscanf(fp, "%d %d %d", &n, &x, &y);
         while (getd(fp, &x, &y)) {
            temp_xmin = (temp_xmin < x ? temp_xmin : x);
            temp_ymin = (temp_ymin < y ? temp_ymin : y);
            temp_xmax = (temp_xmax > x ? temp_xmax : x);
            temp_ymax = (temp_ymax > y ? temp_ymax : y);
         }
         break;
      case 'S':
         fscanf(fp, "%d", &ch);
         ch += 3;
         break;
      case 'Q':
         fscanf(fp, "%d", &cw);
         cw += 3;
         break;
      case 'R':
         fscanf(fp, "%d", &rotate);
         break;
      case 'P':
         n = getstrlength(fp);
         if (!rotate) {
            x += n * cw * mag;
            temp_xmax = (temp_xmax > x ? temp_xmax : x);
            y -= ch * mag;
            temp_ymin = (temp_ymin < y ? temp_ymin : y);
         } else {
            y += n * cw * mag;
            temp_ymax = (temp_ymax > y ? temp_ymax : y);
            x -= ch * mag;
            temp_xmin = (temp_xmin < x ? temp_xmin : x);
         }
         rotate = 0;
         break;
      case '\\':
         fscanf(fp, "%d %d", &temp_plot_xmin, &temp_plot_xmin);
         if (plot_xmin > temp_plot_xmin)
            plot_xmin = temp_plot_xmin;
         if (plot_ymin > temp_plot_ymin)
            plot_ymin = temp_plot_ymin;
         if (plot_xmin < 0)
            plot_xmin = 0;
         if (plot_ymin < 0)
            plot_ymin = 0;
         break;
      case 'Z':
         fscanf(fp, "%d %d", &temp_plot_xmax, &temp_plot_ymax);
         if (plot_xmax < temp_plot_xmax)
            plot_xmax = temp_plot_xmax;
         if (plot_ymax < temp_plot_ymax)
            plot_ymax = temp_plot_ymax;
         if (plot_xmax > xleng)
            plot_xmax = xleng;
         if (plot_ymax > yleng)
            plot_ymax = yleng;
         break;
      default:
         break;
      }
   }

   *xmin = plot_min(temp_xmin, *xmin, plot_xmin);
   *xmax = plot_max(temp_xmax, *xmax, plot_xmax);
   *ymin = plot_min(temp_ymin, *ymin, plot_ymin);
   *ymax = plot_max(temp_ymax, *ymax, plot_ymax);

   unit_length = shrink * PU_PT;

   if (!landscape) {
      *xmin = norm((*xmin + xoffset - bbm.left) * unit_length + MIN_OFFSET);
      *ymin = norm((*ymin + yoffset - bbm.bottom) * unit_length + MIN_OFFSET);
      *xmax = norm((*xmax + xoffset + bbm.right) * unit_length + MAX_OFFSET);
      *ymax = norm((*ymax + yoffset + bbm.top) * unit_length + MAX_OFFSET);
   } else {
      x = *xmin;
      y = *xmax;

      loffset += yoffset;

      *xmin = norm((yleng * unit_length
                    - *ymax + loffset - bbm.bottom) * unit_length + MIN_OFFSET);
      *xmax = norm((yleng * unit_length
                    - *ymin + loffset + bbm.top) * unit_length + MAX_OFFSET);
      loffset = (loffset - yoffset) / 4 + xoffset;
      *ymin = norm((x + loffset - bbm.left) * unit_length + MIN_OFFSET);
      *ymax = norm((y + loffset + bbm.right) * unit_length + MAX_OFFSET);
   }
   rewind(fp);

   return;
}

/* epsf_setup: setup epsf */
void epsf_setup(FILE * fp, float shrink, int xoffset, int yoffset,
                struct bbmargin bbm, int ncopy)
{
   int xmin = 9999, ymin = 9999, xmax = 0, ymax = 0;

   if (!psmode)
      bbox(fp, &xmin, &ymin, &xmax, &ymax, shrink, xoffset, yoffset, bbm);
   epsf_init(&xmin, &ymin, &xmax, &ymax, ncopy);
   epsf_scale(shrink, xoffset, yoffset);

   return;
}

/* epsf_end: terminate epsf */
void epsf_end(void)
{
   if (clip_mode)
      printf("GR\n");
   printf("%%Trailer\n");
   if (psmode)
      printf("%%%%Pages: 1\n");
   printf("showpage\n");
   printf("%%%%EOF\n");

   return;
}
