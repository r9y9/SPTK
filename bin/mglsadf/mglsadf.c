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
*    MGLSA Digital Filter for Speech Synthesis                          *
*                                                                       *
*                                        1996.3  K.Koishida             *
*                                                                       *
*       usage:                                                          *
*               mglsadf [ options ] [ infile ] > stdout                 *
*       options:                                                        *
*               -m m     :  order of mel-generalized cepstrum  [25]     *
*               -a a     :  alpha                              [0.35]   *
*               -c c     :  gamma = -1 / (int) c               [1]      *
*               -p p     :  frame period                       [100]    *
*               -i i     :  interpolation period               [1]      *
*               -t       :  transpose filter                   [FALSE]  *
*               -v       :  inverse filter                     [FALSE]  *
*               -k       :  filtering without gain             [FALSE]  *
*               -P P     :  order of Pade approximation        [4]      *
*       infile:                                                         *
*               mel-generalized cepstral coefficients                   *
*                      , c~(0), c~(1), ..., c~(M),                      *
*       excitation sequence                                             *
*                      , x(0), x(1), ...,                               *
*       stdout:                                                         *
*               filtered sequence                                       *
*                      , y(0), y(1), ...,                               *
*       notice:                                                         *
*               if c==0, MLSA filter is used, P should be 4 or 5        *
*       require:                                                        *
*               mglsadf(), mlsadf(), imglsadf(), mglsadft(), imglsadft()*
*                                                                       *  
************************************************************************/

static char *rcs_id = "$Id: mglsadf.c,v 1.33 2013/12/16 09:02:01 mataki Exp $";


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

#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

/*  Default Values  */
#define ORDER     25
#define ALPHA     0.35
#define STAGE     1
#define FPERIOD   100
#define IPERIOD   1
#define TRANSPOSE FA
#define INVERSE   FA
#define NGAIN     FA
#define PADEORDER 4

char *BOOL[] = { "FALSE", "TRUE" };

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - MGLSA digital filter for speech synthesis\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] mgcfile [ infile ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -m m  : order of mel-generalized cepstrum [%d]\n",
           ORDER);
   fprintf(stderr, "       -a a  : alpha                             [%g]\n",
           ALPHA);
   fprintf(stderr, "       -c c  : gamma = -1 / (int) c              [%d]\n",
           STAGE);
   fprintf(stderr, "       -p p  : frame period                      [%d]\n",
           FPERIOD);
   fprintf(stderr, "       -i i  : interpolation period              [%d]\n",
           IPERIOD);
   fprintf(stderr, "       -t    : transpose filter                  [%s]\n",
           BOOL[TRANSPOSE]);
   fprintf(stderr, "       -v    : inverse filter                    [%s]\n",
           BOOL[INVERSE]);
   fprintf(stderr, "       -k    : filtering without gain            [%s]\n",
           BOOL[NGAIN]);
   fprintf(stderr, "       -P P  : order of Pade approximation       [%d]\n",
           PADEORDER);
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "       filter input (%s)                      [stdin]\n",
           FORMAT);
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       filter output (%s)\n", FORMAT);
   fprintf(stderr, "  mgcfile:\n");
   fprintf(stderr, "       mel-generalized cepstrum (%s)\n", FORMAT);
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       if c==0, MLSA filter is used, P should be 4 or 5\n");
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
   int m = ORDER, fprd = FPERIOD, iprd = IPERIOD, stage = STAGE, pd =
       PADEORDER, i, j;
   Boolean transpose = TRANSPOSE, ngain = NGAIN, inverse = INVERSE;
   FILE *fp = stdin, *fpc = NULL;
   double alpha = ALPHA, gamma = -1 / (double) STAGE, x, *c, *inc, *cc, *d;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':
            m = atoi(*++argv);
            --argc;
            break;
         case 'a':
            alpha = atof(*++argv);
            --argc;
            break;
         case 'c':
            stage = atoi(*++argv);
            --argc;
            break;
         case 'p':
            fprd = atoi(*++argv);
            --argc;
            break;
         case 'i':
            iprd = atoi(*++argv);
            --argc;
            break;
         case 't':
            transpose = 1 - transpose;
            break;
         case 'v':
            inverse = 1 - inverse;
            break;
         case 'k':
            ngain = 1 - ngain;
            break;
         case 'P':
            pd = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (fpc == NULL)
         fpc = getfp(*argv, "rb");
      else
         fp = getfp(*argv, "rb");

   if (fpc == NULL) {
      fprintf(stderr, "%s : Cannot open cepstrum file!\n", cmnd);
      return (1);
   }

   if (inverse) {
      if (stage == 0) {
         fprintf(stderr, "%s : gamma should not equal to 0 in Inverse MGLSA!\n",
                 cmnd);
         usage(1);
      }
   }

   if (stage != 0) {            /* MGLSA */
      gamma = -1 / (double) stage;
   } else {                     /* MLSA */
      if ((pd < 4) || (pd > 5)) {
         fprintf(stderr, "%s : Order of Pade approximation should be 4 or 5!\n",
                 cmnd);
         return (1);
      }
   }

   c = (stage != 0) ? dgetmem(m + m + m + 3 + (m + 1) * stage)  /* MGLSA */
       : dgetmem(3 * (m + 1) + 3 * (pd + 1) + pd * (m + 2));    /* MLSA  */
   cc = c + m + 1;
   inc = cc + m + 1;
   d = inc + m + 1;

   if (freadf(c, sizeof(*c), m + 1, fpc) != m + 1)
      return (1);
   mc2b(c, c, m, alpha);
   if (stage != 0) {            /* MGLSA */
      gnorm(c, c, m, gamma);
      c[0] = log(c[0]);
      for (i = 1; i <= m; i++)
         c[i] *= gamma;
   }


   for (;;) {
      if (freadf(cc, sizeof(*cc), m + 1, fpc) != m + 1)
         return (0);
      mc2b(cc, cc, m, alpha);
      if (stage != 0) {
         gnorm(cc, cc, m, gamma);
         cc[0] = log(cc[0]);
         for (i = 1; i <= m; i++)
            cc[i] *= gamma;
      }

      for (i = 0; i <= m; i++)
         inc[i] = (cc[i] - c[i]) * iprd / fprd;

      for (j = fprd, i = (iprd + 1) / 2; j--;) {
         if (freadf(&x, sizeof(x), 1, fp) != 1)
            return (0);


         if (inverse) {         /* IMGLSA */
            if (!ngain)
               x /= exp(c[0]);
            if (transpose)
               x = imglsadft(x, c, m, alpha, stage, d);
            else
               x = imglsadf(x, c, m, alpha, stage, d);
         } else {
            if (stage != 0) {   /* MGLSA */
               if (!ngain)
                  x *= exp(c[0]);
               if (transpose)
                  x = mglsadft(x, c, m, alpha, stage, d);
               else
                  x = mglsadf(x, c, m, alpha, stage, d);
            } else {            /* MLSA */
               if (!ngain)
                  x *= exp(c[0]);
               x = mlsadf(x, c, m, alpha, pd, d);
            }
         }

         fwritef(&x, sizeof(x), 1, stdout);

         if (!--i) {
            for (i = 0; i <= m; i++)
               c[i] += inc[i];
            i = iprd;
         }
      }

      movem(cc, c, sizeof(*cc), m + 1);
   }

   return (0);
}
