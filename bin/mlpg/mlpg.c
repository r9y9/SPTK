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

/****************************************************************************************
*                                                                                       *
*    ML-based Parameter Generation from PDFs                                            *
*                                                                                       *
*                                    2000.4  T.Masuko                                   *
*                                                                                       *
*       usage:                                                                          *
*               mlpg [ options ] [infile] > stdout                                      *
*       options:                                                                        *
*               -m m              : order of vector                              [25]   *
*               -l l              : length of vector                             [m+1]  *
*               -d fn             : filename of delta coefficients               [N/A]  *
*               -d coef [coef...] : delta coefficients                           [N/A]  *
*               -r n w1 [w2]      : number and width of regression coefficients  [N/A]  *
*               -i i              : type of input PDFs                           [0]    *
*                                    0: ( m       , U      )                            *
*                                    1: ( m       , U^{-1} )                            *
*                                    2: ( mU^{-1} , U^{-1} )                            *
*               -s s              : range of influenced frames                   [30]   *
*               -h                : print this message                                  *
*       infile:                                                                         *
*              PDF sequence                                                             *
*              ex.)   , m(0), m(1), ..., m((m+1)*d-1),                                  *
*                     U(0), U(1), ..., U((m+1)*d-1),                                    *
*       stdout:                                                                         *
*              parameter sequence                                                       *
*                     , c(0), c(1), ..., c_1(m),                                        *
*                                                                                       *
****************************************************************************************/

static char *rcs_id = "$Id: mlpg.c,v 1.36 2014/12/11 08:30:42 uratec Exp $";


/* Standard C Libraries */
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

#include <ctype.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/* default values */
#define  ORDER  25
#define  ITYPE  0
#define  RANGE  30


/*  Command Name  */
char *cmnd;


/*  Other Definitions  */
#ifdef DOUBLE
typedef double real;
#else
typedef float real;
#endif

#define LENGTH  256
#define INFTY  ((double) 1.0e+38)
#define INFTY2  ((double) 1.0e+19)
#define INVINF  ((double) 1.0e-38)
#define INVINF2  ((double) 1.0e-19)

#define WLEFT  0
#define WRIGHT  1

#define abs(x)  ((x)>0.0 ? (x) : -(x))
#define sign(x)  ((x) >= 0.0 ? 1 : -1)
#define finv(x)  (abs(x) <= INVINF2 ? sign(x)*INFTY : (abs(x) >= INFTY2 ? 0 : 1.0/(x)))
#define min(x, y) ((x)<(y) ? (x) : (y))

typedef struct _DWin {
   int num;                     /* number of static + deltas */
   int calccoef;                /* calculate regression coefficients */
   char **fn;                   /* delta window coefficient file */
   int **width;                 /* width [0..num-1][0(left) 1(right)] */
   double **coef;               /* coefficient [0..num-1][length[0]..length[1]] */
   int maxw[2];                 /* max width [0(left) 1(right)] */
} DWin;

typedef struct _SMatrix {
   double **mseq;               /* sequence of mean vector */
   double **ivseq;              /* sequence of invarsed variance vector */
   double ***P;                 /* matrix P[th][tv][m] */
   double **c;                  /* parameter c */
   double **pi;
   double **k;
   int t;                       /* time index */
   int length;                  /* matrix length (must be power of 2) */
   unsigned int mask;           /* length - 1 */
} SMatrix;

typedef struct _PStream {
   int vSize;                   /* data vector size */
   int order;                   /* order of cepstrum */
   int range;
   DWin dw;
   double *mean;                /* input mean vector */
   double *ivar;                /* input inversed variance vector */
   double *par;                 /* output parameter vector */
   int iType;                   /* type of input PDFs */
   /*   0: ( m       , U      ) */
   /*   1: ( m       , U^{-1} ) */
   /*   2: ( mU^{-1} , U^{-1} ) */
   SMatrix sm;
} PStream;

/*  Required Functions  */
void init_pstream(PStream * pst);
void init_dwin(PStream * pst);
double *dcalloc(int x, int xoff);
double **ddcalloc(int x, int y, int xoff, int yoff);
double ***dddcalloc(int x, int y, int z, int xoff, int yoff, int zoff);
double *mlpg(PStream * pst);
int doupdate(PStream * pst, int d);
void calc_pi(PStream * pst, int d);
void calc_k(PStream * pst, int d);
void update_P(PStream * pst);
void update_c(PStream * pst, int d);


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - obtain parameter sequence from PDF sequence\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] [infile] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr,
           "       -m m              : order of vector                              [%d]\n",
           ORDER);
   fprintf(stderr,
           "       -l l              : length of vector                             [m+1]\n");
   fprintf(stderr,
           "       -d fn             : filename of delta coefficients               [N/A]\n");
   fprintf(stderr,
           "       -d coef [coef...] : delta coefficients                           [N/A]\n");
   fprintf(stderr,
           "       -r n t1 [t2]      : number and width of regression coefficients  [N/A]\n");
   fprintf(stderr,
           "       -i i              : type of input PDFs                           [%d]\n",
           ITYPE);
   fprintf(stderr, "                             0: ( m       , U      )\n");
   fprintf(stderr, "                             1: ( m       , U^{-1} )\n");
   fprintf(stderr, "                             2: ( mU^{-1} , U^{-1} )\n");
   fprintf(stderr,
           "       -s s              : range of influenced frames                   [%d]\n",
           RANGE);
   fprintf(stderr, "       -h                : print this message\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       PDF sequence                                                     [stdin]\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       parameter sequence\n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr,
           "       1) Option '-d' may be repeated to use multiple delta parameters.\n");
   fprintf(stderr,
           "       2) Options '-d' and '-r' should not be defined simultaneously.\n");
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
   FILE *pdffp = stdin, *parfp = stdout;
   int nframe, delay;
   char *coef;
   int coeflen;
   PStream pst;
   int i, j;
   void init_pstream(PStream *);
   double *mlpg(PStream *);

   pst.order = ORDER;
   pst.range = RANGE;
   pst.iType = ITYPE;
   pst.dw.fn = (char **) calloc(sizeof(char *), argc);
   pst.dw.num = 1;
   pst.dw.calccoef = -1;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'd':
            if (pst.dw.calccoef == 1) {
               fprintf(stderr,
                       "%s : Options '-r' and '-d' should not be defined simultaneously!\n",
                       cmnd);
               return (1);
            }
            pst.dw.calccoef = 0;
            if (isfloat(*++argv)) {
               coeflen = 0;
               for (i = 0; (i < argc - 1) && isfloat(argv[i]); i++) {
                  coeflen += strlen(argv[i]) + 1;
               }
               coeflen += 1;
               coef = pst.dw.fn[pst.dw.num] = getmem(coeflen, sizeof(char));
               for (j = 0; j < i; j++) {
                  sprintf(coef, " %s", *argv);
                  coef += strlen(*argv) + 1;
                  if (j < i - 1) {
                     argv++;
                     argc--;
                  }
               }
            } else {
               pst.dw.fn[pst.dw.num] = *argv;
            }
            pst.dw.num++;
            --argc;
            break;
         case 'r':
            if (pst.dw.calccoef == 0) {
               fprintf(stderr,
                       "%s : Options '-r' and '-d' should not be defined simultaneously!\n",
                       cmnd);
               return (1);
            }
            pst.dw.calccoef = 1;
            coeflen = atoi(*++argv);
            --argc;
            if ((coeflen != 1) && (coeflen != 2)) {
               fprintf(stderr,
                       "%s : Number of delta parameter should be 1 or 2!\n",
                       cmnd);
               return (1);
            }
            if (argc <= 1) {
               fprintf(stderr,
                       "%s : Window size for delta parameter required!\n",
                       cmnd);
               return (1);
            }
            pst.dw.fn[pst.dw.num] = *++argv;
            pst.dw.num++;
            --argc;
            if (coeflen == 2) {
               if (argc <= 1) {
                  fprintf(stderr,
                          "%s : Window size for delta-delta parameter required!\n",
                          cmnd);
                  return (1);
               }
               pst.dw.fn[pst.dw.num] = *++argv;
               pst.dw.num++;
               --argc;
            }
            break;
         case 'm':
            pst.order = atoi(*++argv);
            --argc;
            break;
         case 'l':
            pst.order = atoi(*++argv) - 1;
            --argc;
            break;
         case 'i':
            pst.iType = atoi(*++argv);
            --argc;
            break;
         case 's':
            pst.range = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else
         pdffp = getfp(*argv, "rb");
   }

   init_pstream(&pst);

   delay = pst.range + pst.dw.maxw[WRIGHT];
   nframe = 0;
   while (freadf(pst.mean, sizeof(*(pst.mean)), pst.vSize * 2, pdffp) ==
          pst.vSize * 2) {
      if (pst.dw.num == 1)
         fwritef(pst.mean, sizeof(*pst.mean), pst.order + 1, parfp);
      else {
         if (pst.iType == 0)
            for (i = 0; i < pst.vSize; i++)
               pst.ivar[i] = finv(pst.ivar[i]);
         mlpg(&pst);
         if (nframe >= delay)
            fwritef(pst.par, sizeof(*(pst.par)), pst.order + 1, parfp);
      }
      nframe++;
   }

   if (pst.dw.num > 1) {
      for (i = 0; i < pst.vSize; i++) {
         pst.mean[i] = 0.0;
         pst.ivar[i] = 0.0;
      }
      for (i = 0; i < min(nframe, delay); i++) {
         mlpg(&pst);
         fwritef(pst.par, sizeof(*(pst.par)), pst.order + 1, parfp);
      }
   }

   return (0);
}

void init_pstream(PStream * pst)
{
   void init_dwin(PStream *);
   double *dcalloc(int, int);
   double **ddcalloc(int, int, int, int);
   double ***dddcalloc(int, int, int, int, int, int);
   int half, full;
   int i, m;

   init_dwin(pst);

   half = pst->range * 2;
   full = pst->range * 4 + 1;

   pst->vSize = (pst->order + 1) * pst->dw.num;

   pst->sm.length = LENGTH;
   while (pst->sm.length < pst->range + pst->dw.maxw[WRIGHT])
      pst->sm.length *= 2;

   pst->mean = dcalloc(pst->vSize * 2, 0);
   pst->ivar = pst->mean + pst->vSize;

   pst->sm.mseq = ddcalloc(pst->sm.length, pst->vSize, 0, 0);
   pst->sm.ivseq = ddcalloc(pst->sm.length, pst->vSize, 0, 0);

   pst->sm.c = ddcalloc(pst->sm.length, pst->order + 1, 0, 0);
   pst->sm.P = dddcalloc(full, pst->sm.length, pst->order + 1, half, 0, 0);

   pst->sm.pi =
       ddcalloc(pst->range + pst->dw.maxw[WRIGHT] + 1, pst->order + 1,
                pst->range, 0);
   pst->sm.k =
       ddcalloc(pst->range + pst->dw.maxw[WRIGHT] + 1, pst->order + 1,
                pst->range, 0);

   for (i = 0; i < pst->sm.length; i++)
      for (m = 0; m < pst->vSize; m++)
         pst->sm.ivseq[i][m] = 0.0;

   for (i = 0; i < pst->sm.length; i++)
      for (m = 0; m <= pst->order; m++)
         pst->sm.P[0][i][m] = INFTY;

   pst->sm.t = pst->range - 1;
   pst->sm.mask = pst->sm.length - 1;

   return;
}


void init_dwin(PStream * pst)
{
   double *dcalloc(int, int);
   int i, j;
   int fsize, leng;
   double a0, a1, a2;
   FILE *fp;

   /* memory allocation */
   if ((pst->dw.width = (int **) calloc(pst->dw.num, sizeof(int *))) == NULL) {
      fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
      exit(1);
   }
   for (i = 0; i < pst->dw.num; i++)
      if ((pst->dw.width[i] = (int *) calloc(2, sizeof(int))) == NULL) {
         fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
         exit(1);
      }
   if ((pst->dw.coef =
        (double **) calloc(pst->dw.num, sizeof(double *))) == NULL) {
      fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
      exit(1);
   }

   /* window for static parameter */
   pst->dw.width[0][WLEFT] = pst->dw.width[0][WRIGHT] = 0;
   pst->dw.coef[0] = dcalloc(1, 0);
   pst->dw.coef[0][0] = 1;

   /* set delta coefficients */
   if (pst->dw.calccoef == 0) {
      for (i = 1; i < pst->dw.num; i++) {
         if (pst->dw.fn[i][0] == ' ') {
            fsize = str2darray(pst->dw.fn[i], &(pst->dw.coef[i]));
         } else {
            /* read from file */
            fp = getfp(pst->dw.fn[i], "rb");

            /* check the number of coefficients */
            fseek(fp, 0L, 2);
            fsize = ftell(fp) / sizeof(real);
            fseek(fp, 0L, 0);

            /* read coefficients */
            pst->dw.coef[i] = dcalloc(fsize, 0);
            freadf(pst->dw.coef[i], sizeof(**(pst->dw.coef)), fsize, fp);
         }

         /* set pointer */
         leng = fsize / 2;
         pst->dw.coef[i] += leng;
         pst->dw.width[i][WLEFT] = -leng;
         pst->dw.width[i][WRIGHT] = leng;
         if (fsize % 2 == 0)
            pst->dw.width[i][WRIGHT]--;
      }
   } else if (pst->dw.calccoef == 1) {
      for (i = 1; i < pst->dw.num; i++) {
         leng = atoi(pst->dw.fn[i]);
         if (leng < 1) {
            fprintf(stderr,
                    "%s : Width for regression coefficient shuould be more than 1!\n",
                    cmnd);
            exit(1);
         }
         pst->dw.width[i][WLEFT] = -leng;
         pst->dw.width[i][WRIGHT] = leng;
         pst->dw.coef[i] = dcalloc(leng * 2 + 1, 0);
         pst->dw.coef[i] += leng;
      }

      leng = atoi(pst->dw.fn[1]);
      for (a1 = 0, j = -leng; j <= leng; a1 += j * j, j++);
      for (j = -leng; j <= leng; j++)
         pst->dw.coef[1][j] = (double) j / (double) a1;

      if (pst->dw.num > 2) {
         leng = atoi(pst->dw.fn[2]);
         for (a0 = a1 = a2 = 0, j = -leng; j <= leng;
              a0++, a1 += j * j, a2 += j * j * j * j, j++);
         for (j = -leng; j <= leng; j++)
            pst->dw.coef[2][j] =
                ((double) (a0 * j * j - a1)) / ((double) (a2 * a0 - a1 * a1)) /
                2;
      }
   }

   pst->dw.maxw[WLEFT] = pst->dw.maxw[WRIGHT] = 0;
   for (i = 0; i < pst->dw.num; i++) {
      if (pst->dw.maxw[WLEFT] > pst->dw.width[i][WLEFT])
         pst->dw.maxw[WLEFT] = pst->dw.width[i][WLEFT];
      if (pst->dw.maxw[WRIGHT] < pst->dw.width[i][WRIGHT])
         pst->dw.maxw[WRIGHT] = pst->dw.width[i][WRIGHT];
   }

   return;
}


double *dcalloc(int x, int xoff)
{
   double *ptr;

   if ((ptr = (double *) calloc(x, sizeof(*ptr))) == NULL) {
      fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
      exit(1);
   }
   ptr += xoff;
   return (ptr);
}


double **ddcalloc(int x, int y, int xoff, int yoff)
{
   double *dcalloc(int, int);
   double **ptr;
   int i;

   if ((ptr = (double **) calloc(x, sizeof(*ptr))) == NULL) {
      fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
      exit(1);
   }
   for (i = 0; i < x; i++)
      ptr[i] = dcalloc(y, yoff);
   ptr += xoff;
   return (ptr);
}


double ***dddcalloc(int x, int y, int z, int xoff, int yoff, int zoff)
{
   double **ddcalloc(int, int, int, int);
   double ***ptr;
   int i;

   if ((ptr = (double ***) calloc(x, sizeof(*ptr))) == NULL) {
      fprintf(stderr, "%s : Cannot allocate memory!\n", cmnd);
      exit(1);
   }
   for (i = 0; i < x; i++)
      ptr[i] = ddcalloc(y, z, yoff, zoff);
   ptr += xoff;
   return (ptr);
}

/*--------------------------------------------------------------------*/

double *mlpg(PStream * pst)
{
   int doupdate(PStream *, int);
   void calc_pi(PStream *, int);
   void calc_k(PStream *, int);
   void update_P(PStream *);
   void update_c(PStream *, int);
   int tcur, tmin, tmax;
   int d, m, u;

   pst->sm.t++;
   tcur = pst->sm.t & pst->sm.mask;
   tmin = (pst->sm.t - pst->range) & pst->sm.mask;
   tmax = (pst->sm.t + pst->dw.maxw[WRIGHT]) & pst->sm.mask;

   for (u = -pst->range * 2; u <= pst->range * 2; u++) {
      for (m = 0; m <= pst->order; m++)
         pst->sm.P[u][tmax][m] = 0.0;
   }
   for (m = 0; m < pst->vSize; m++) {
      pst->sm.mseq[tmax][m] = pst->mean[m];
      pst->sm.ivseq[tmax][m] = pst->ivar[m];
   }
   for (m = 0; m <= pst->order; m++) {
      if (pst->iType != 2)
         pst->sm.c[tmax][m] = pst->mean[m];
      else
         pst->sm.c[tmax][m] = pst->mean[m] * finv(pst->ivar[m]);
      pst->sm.P[0][tmax][m] = finv(pst->ivar[m]);
   }

   for (d = 1; d < pst->dw.num; d++) {
      if (doupdate(pst, d)) {
         calc_pi(pst, d);
         calc_k(pst, d);
         update_P(pst);
         update_c(pst, d);
      }
   }
   pst->par = pst->sm.c[tmin];
   return (pst->par);
}


int doupdate(PStream * pst, int d)
{
   int j;

   if (pst->sm.ivseq[pst->sm.t & pst->sm.mask][(pst->order + 1) * d] == 0.0)
      return (0);
   for (j = pst->dw.width[d][WLEFT]; j <= pst->dw.width[d][WRIGHT]; j++)
      if (pst->sm.P[0][(pst->sm.t + j) & pst->sm.mask][0] == INFTY)
         return (0);
   return (1);
}


void calc_pi(PStream * pst, int d)
{
   int j, m, u;

   for (m = 0; m <= pst->order; m++)
      for (u = -pst->range; u <= pst->dw.maxw[WRIGHT]; u++) {
         pst->sm.pi[u][m] = 0.0;
         for (j = pst->dw.width[d][WLEFT]; j <= pst->dw.width[d][WRIGHT]; j++)
            pst->sm.pi[u][m] +=
                pst->sm.P[u -
                          j][(pst->sm.t +
                              j) & pst->sm.mask][m] * pst->dw.coef[d][j];
      }

   return;
}


void calc_k(PStream * pst, int d)
{
   int j, m, u;
   double *ivar, x;

   ivar = pst->sm.ivseq[pst->sm.t & pst->sm.mask] + (pst->order + 1) * d;
   for (m = 0; m <= pst->order; m++) {
      x = 0.0;
      for (j = pst->dw.width[d][WLEFT]; j <= pst->dw.width[d][WRIGHT]; j++)
         x += pst->dw.coef[d][j] * pst->sm.pi[j][m];
      x = ivar[m] / (1.0 + ivar[m] * x);
      for (u = -pst->range; u <= pst->dw.maxw[WRIGHT]; u++) {
         pst->sm.k[u][m] = pst->sm.pi[u][m] * x;
      }
   }

   return;
}


void update_P(PStream * pst)
{
   int m, u, v;

   for (m = 0; m <= pst->order; m++)
      for (u = -pst->range; u <= pst->dw.maxw[WRIGHT]; u++)
         for (v = u; v <= pst->dw.maxw[WRIGHT]; v++) {
            pst->sm.P[v - u][(pst->sm.t + u) & pst->sm.mask][m] -=
                pst->sm.k[v][m] * pst->sm.pi[u][m];
            if (v != u)
               pst->sm.P[u - v][(pst->sm.t + v) & pst->sm.mask][m] =
                   pst->sm.P[v - u][(pst->sm.t + u) & pst->sm.mask][m];
         }

   return;
}


void update_c(PStream * pst, int d)
{
   int j, m, u;
   double *mean, *ivar, x;

   ivar = pst->sm.ivseq[pst->sm.t & pst->sm.mask] + (pst->order + 1) * d;
   mean = pst->sm.mseq[pst->sm.t & pst->sm.mask] + (pst->order + 1) * d;
   for (m = 0; m <= pst->order; m++) {
      x = mean[m];
      if (pst->iType == 2)
         x *= finv(ivar[m]);
      for (j = pst->dw.width[d][WLEFT]; j <= pst->dw.width[d][WRIGHT]; j++)
         x -= pst->dw.coef[d][j] * pst->sm.c[(pst->sm.t + j) & pst->sm.mask][m];
      for (u = -pst->range; u <= pst->dw.maxw[WRIGHT]; u++)
         pst->sm.c[(pst->sm.t + u) & pst->sm.mask][m] += pst->sm.k[u][m] * x;
   }

   return;
}
