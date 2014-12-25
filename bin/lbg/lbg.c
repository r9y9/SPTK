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

/******************************************************************************
*                                                                             *
*     LBG Algorithm for Vector Quantizer Design                               *
*                                                                             *
*                                             1996. 4  K.Koishida             *
*                                             2010. 8  A.Tamamori             *
*                                                                             *
*        usage:                                                               *
*                lbg [ options ] [ indexfile ] < stdin > stdout               *
*        options:                                                             *
*                -l l      :  length of vector                  [26]          *
*                -n n      :  order of vector                   [25]          *
*                -s s      :  initial codebook size             [1]           *
*                -e e      :  final codebook size               [256]         *
*                -F F      :  initial codebook filename         [NULL]        *
*                -i i      :  maximum number of iteration       [1000]        *
*                -m m      :  minimum num. of training          [NULL]        *
*                             vectors for each cell             [1]           *
*                -S S      :  seed for each centroid            [NULL]        *
*                -c c      :  type of exception procedure       [1]           *
*                             for centroid update                             *
*                             c = 1 : split the centroid                      *
*                                     with most train. vector                 *
*                             c = 2 : split the vector which                  *
*                                     internally divide two                   *
*                                     centroids sharing the                   *
*                                     same parent centroid.                   *
*                (level 2)                                                    *
*                -d d      :  end condition                     [0.0001]      *
*                -r r      :  splitting factor                  [0.0001]      *
*       infile:                                                               *
*                training vector (stdin)                                      *
*                        x(0), x(1), ... x(t*l-1)                             *
*       stdout:                                                               *
*               trained codebook                                              *
*                        cb'(0), cb'(1), ... cb(l*e-1)                        *
*       ifile: (if s>1)                                                       *
*              initial codebook (cbfile)                                      *
*                        cb(0), cb(1), ... cb(l*s-1)                          *
*       indexfile:                                                            *
*              VQ index of training vector (int)                              *
*                        index(0), index(1), ... index(t-1)                   *
*       sfile:                                                                *
*              seq. of seed given to each centroid (int)                      *
*                        seed(0), seed(1), ... , seed(e-1)                    *
*       notice:                                                               *
*              codebook size (s and e) must be power of 2                     *
*              -t option can be omitted, when input from redirect             *
*      require:                                                               *
*              lbg(), vq()                                                    *
*                                                                             *
******************************************************************************/

static char *rcs_id = "$Id: lbg.c,v 1.30 2014/12/11 08:30:38 uratec Exp $";


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

/*  Default Values  */
#define LENG 26
#define TNUMBER -1
#define ICBSIZE 1
#define ECBSIZE 256
#define DELTA 0.0001
#define END 0.0001
#define MINTRAIN 1
#define SEED 1
#define CENTUP 1
#define ITER 1000

#define MAXVALUE 1e23
#define abs(x)  ( (x<0) ? (-(x)) : (x) )

/*  Command Name  */
char *cmnd;

typedef struct _float_list {
   float *f;
   struct _float_list *next;
} float_list;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - LBG algorithm for vector quantizer design \n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [ ifile ]<stdin>stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -l l      : length of vector                                     [%d]\n",
           LENG);
   fprintf(stderr,
           "       -n n      : order of vector                                      [%d]\n",
           LENG - 1);
   fprintf(stderr,
           "       -s s      : initial codebook size                                [%d]\n",
           ICBSIZE);
   fprintf(stderr,
           "       -e e      : final codebook size                                  [%d]\n",
           ECBSIZE);
   fprintf(stderr,
           "       -F F      : initial codebook filename                            [NULL]\n");
   fprintf(stderr,
           "       -i i      : maximum number of iteration for centroid update      [%d]\n",
           ITER);
   fprintf(stderr,
           "       -m m      : minimum number of training vectors for each cell     [%d]\n",
           MINTRAIN);
   fprintf(stderr,
           "       -S S      : seed for each centroid                               [%d]\n",
           SEED);
   fprintf(stderr,
           "       -c c      : type of exception procedure for centorid update      [%d]\n",
           CENTUP);
   fprintf(stderr,
           "                   c = 1 : split the centroid with most training vector\n");
   fprintf(stderr,
           "                   c = 2 : split the vector which internally divide \n");
   fprintf(stderr,
           "                           the two centroids sharing the same parent\n");
   fprintf(stderr, "                           centroids\n");
   fprintf(stderr, "       -h        : print this message\n");
   fprintf(stderr, "     (level 2)\n");
   fprintf(stderr, "       -d d  : end condition             [%g]\n", END);
   fprintf(stderr, "       -r r  : splitting factor          [%g]\n", DELTA);
   fprintf(stderr, "  stdin:\n");
   fprintf(stderr, "       data sequence (%s)\n", FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       codebook (%s)\n", FORMAT);
   fprintf(stderr, "  ifile:\n");
   fprintf(stderr, "       index (int)\n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       codebook size (s and e) must be power of 2\n");
   fprintf(stderr,
           "       -t option can be omitted, when input from redirect\n");
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}


int main(int argc, char **argv)
{
   int l = LENG, icbsize = ICBSIZE, ecbsize = ECBSIZE, iter = ITER, tnum =
       TNUMBER, seed = SEED, csize, i, j, *tindex, mintnum =
       MINTRAIN, centup = CENTUP;
   FILE *fp = stdin, *fpi = NULL, *fpcb = NULL;
   double delta = DELTA, minerr = END, *x, *cb, *icb;
   double *p;
   float_list *top, *cur, *prev, *tmpf, *tmpff;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            l = atoi(*++argv);
            --argc;
            break;
         case 'n':
            l = atoi(*++argv) + 1;
            --argc;
            break;
         case 's':
            icbsize = atoi(*++argv);
            --argc;
            break;
         case 'e':
            ecbsize = atoi(*++argv);
            --argc;
            break;
         case 'd':
            minerr = atof(*++argv);
            --argc;
            break;
         case 'r':
            delta = atof(*++argv);
            --argc;
            break;
         case 'F':
            fpcb = getfp(*++argv, "rb");
            --argc;
            break;
         case 'm':
            mintnum = atoi(*++argv);
            --argc;
            break;
         case 'S':
            seed = atoi(*++argv);
            --argc;
            break;
         case 'c':
            centup = atoi(*++argv);
            --argc;
            break;
         case 'i':
            iter = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         fpi = getfp(*argv, "wb");

   /* -- Count number of input vectors and read -- */
   x = dgetmem(l);
   top = prev = (float_list *) malloc(sizeof(float_list));
   top->f = fgetmem(l);
   tnum = 0;
   prev->next = NULL;
   while (freadf(x, sizeof(*x), l, fp) == l) {
      cur = (float_list *) malloc(sizeof(float_list));
      cur->f = fgetmem(l);
      for (i = 0; i < l; i++) {
         cur->f[i] = (float) x[i];
      }
      tnum++;
      prev->next = cur;
      cur->next = NULL;
      prev = cur;
   }
   free(x);
   x = dgetmem(tnum * l);
   for (i = 0, tmpf = top->next; tmpf != NULL; i++, tmpf = tmpff) {
      for (j = 0; j < l; j++) {
         x[i * l + j] = tmpf->f[j];
      }
      tmpff = tmpf->next;
      free(tmpf->f);
      free(tmpf);
   }
   free(top);

   csize = ecbsize * l;
   cb = dgetmem(csize);

   if (icbsize == 1) {
      icb = dgetmem(l);
      fillz(icb, sizeof(*icb), l);
      for (i = 0, p = x; i < tnum; i++)
         for (j = 0; j < l; j++)
            icb[j] += *p++;

      for (j = 0; j < l; j++)
         icb[j] /= (double) tnum;
   } else {
      icb = dgetmem(icbsize * l);
      if (fpcb == NULL) {
         fprintf(stderr, "%s : initial codebook filename must be specified!\n",
                 cmnd);
         return (1);
      }
      if (freadf(icb, sizeof(*icb), icbsize * l, fpcb) != icbsize * l) {
         fprintf(stderr, "%s : Size error of initial codebook!\n", cmnd);
         return (1);
      }
   }

   lbg(x, l, tnum, icb, icbsize, cb, ecbsize, iter, mintnum, seed, centup,
       delta, minerr);

   fwritef(cb, sizeof(*cb), csize, stdout);

   if (fpi != NULL) {
      tindex = (int *) dgetmem(tnum);
      for (i = 0, p = x; i < tnum; i++, p += l)
         tindex[i] = vq(p, cb, l, ecbsize);

      fwritex(tindex, sizeof(*tindex), tnum, fpi);
   }

   return (0);
}
