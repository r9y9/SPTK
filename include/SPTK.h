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
/*                1996-2016  Nagoya Institute of Technology          */
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

/***********************************************************
   $Id: SPTK.h,v 1.66 2016/12/25 05:00:20 uratec Exp $

   Speech Signal Processing Toolkit
   SPTK.h
***********************************************************/

#ifndef SPTK_H_
#define SPTK_H_

#include <stdio.h>

#ifndef DLLEXPORT
#  ifdef _WIN32
#      define DLLEXPORT __declspec(dllexport)
#    else
#      define DLLEXPORT
#  endif
#endif

#ifndef PI
#define PI  3.14159265358979323846
#endif                          /* PI */

#ifndef PI2
#define PI2 6.28318530717958647692
#endif                          /* PI2 */

#ifndef M_PI
#define M_PI  3.1415926535897932384626433832795
#endif                          /* M_PI */

#ifndef M_2PI
#define M_2PI 6.2831853071795864769252867665590
#endif                          /* M_2PI */

#define LN_TO_LOG 4.3429448190325182765

#define LZERO (-1.0e+10)
#define LSMALL (-0.5e+10)

/* #ifndef ABS(x) */
#define ABS(x) ((x<0.0) ? -x : x)
/* #endif */

#ifdef __BIG_ENDIAN
#if __BYTE_ORDER == __BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif
#endif

/* enum for Boolean */
typedef enum _Boolean { FA, TR } Boolean;

/* enum for window type */
typedef enum _Window {
   BLACKMAN,
   HAMMING,
   HANNING,
   BARTLETT,
   TRAPEZOID,
   RECTANGULAR
} Window;

/* struct for Complex */
typedef struct {
   double re;
   double im;
} Complex;

/* struct for Gaussian distribution */
typedef struct _Gauss {
   double *mean;
   double *var;
   double **cov;
   double **inv;
   double gconst;
} Gauss;

/* structure for GMM */
typedef struct _GMM {
   int nmix;
   int dim;
   Boolean full;
   double *weight;
   Gauss *gauss;
} GMM;

typedef struct _deltawindow {
   size_t win_size;
   size_t win_max_width;
   int *win_l_width;
   int *win_r_width;
   double **win_coefficient;
} DELTAWINDOW;

/* structure for wavsplit and wavjoin */
typedef struct _wavfile {
   int file_size;               /* file size */
   int fmt_chunk_size;          /* size of 'fmt ' chunk (byte) */
   int data_chunk_size;         /* size of 'data' chunk (byte) */
   short format_id;             /* format ID (PCM(1) or IEEE float(3)) */
   short channel_num;           /* mono:1: stereo:2 */
   int sample_freq;             /* sampling frequency (Hz) */
   int byte_per_sec;            /* byte per second */
   short block_size;            /* 16bit, mono => 16bit*1=2byte */
   short bit_per_sample;        /* bit per sample */
   short extended_size;         /* size of 'extension' */

   char input_data_type;
   char format_type;

   char *data;                  /* waveform data */

} Wavfile;

typedef struct _filelist {
   int num;
   char **name;
} Filelist;

/* library routines */
DLLEXPORT double agexp(double r, double x, double y);
DLLEXPORT int cholesky(double *c, double *a, double *b, const int n, double eps);
DLLEXPORT int freada(double *p, const int bl, FILE * fp);
DLLEXPORT int fwritex(void *ptr, const size_t size, const int nitems, FILE * fp);
DLLEXPORT int freadx(void *ptr, const size_t size, const int nitems, FILE * fp);
DLLEXPORT int fwritef(double *ptr, const size_t size, const int nitems, FILE * fp);
DLLEXPORT int freadf(double *ptr, const size_t size, const int nitems, FILE * fp);
DLLEXPORT int fwrite_little_endian(void *ptr, const size_t size,
                         const size_t n, FILE * fp);
DLLEXPORT void fillz(void *ptr, const size_t size, const int nitem);
DLLEXPORT FILE *getfp(char *name, char *opt);
DLLEXPORT short *sgetmem(const int leng);
DLLEXPORT long *lgetmem(const int leng);
DLLEXPORT double *dgetmem(const int leng);
DLLEXPORT float *fgetmem(const int leng);
/* real *rgetmem (const int leng); */
DLLEXPORT float **ffgetmem(const int leng);
DLLEXPORT double **ddgetmem(const int leng1, const int leng2);
DLLEXPORT char *getmem(const size_t leng, const size_t size);
DLLEXPORT double gexp(const double r, const double x);
DLLEXPORT double glog(const double r, const double x);
DLLEXPORT int ifftr(double *x, double *y, const int l);
DLLEXPORT double invert(double **mat, double **inv, const int n);
DLLEXPORT void multim(double x[], const int xx, const int xy, double y[], const int yx,
            const int yy, double a[]);
DLLEXPORT void addm(double x[], double y[], const int xx, const int yy, double a[]);
DLLEXPORT void movem(void *a, void *b, const size_t size, const int nitem);
DLLEXPORT int mseq(void);
DLLEXPORT int theq(double *t, double *h, double *a, double *b, const int n, double eps);
DLLEXPORT int toeplitz(double *t, double *a, double *b, const int n, double eps);


/* tool routines */
DLLEXPORT double acep(double x, double *c, const int m, const double lambda,
            const double step, const double tau, const int pd,
            const double eps);
DLLEXPORT void acorr(double *x, int l, double *r, const int np);
DLLEXPORT double agcep(double x, double *c, const int m, const int stage,
             const double lambda, const double step, const double tau,
             const double eps);
DLLEXPORT double amcep(double x, double *b, const int m, const double a,
             const double lambda, const double step, const double tau,
             const int pd, const double eps);
DLLEXPORT void phidf(const double x, const int m, double a, double *d);
DLLEXPORT double average(double *x, const int n);
DLLEXPORT void vaverage(double *x, const int l, const int num, double *ave);
DLLEXPORT void b2mc(double *b, double *mc, int m, const double a);
DLLEXPORT void c2acr(double *c, const int m1, double *r, const int m2, const int flng);
DLLEXPORT void c2ir(double *c, const int nc, double *h, const int leng);
DLLEXPORT void c2ndps(double *c, const int m, double *n, const int l);
DLLEXPORT void ic2ir(double *h, const int leng, double *c, const int nc);
DLLEXPORT void c2sp(double *c, const int m, double *x, double *y, const int l);
DLLEXPORT void clip(double *x, const int l, const double min, const double max,
          double *y);
DLLEXPORT int dft(double *pReal, double *pImag, const int nDFTLength);
DLLEXPORT void dct(double *in, double *out, const int size, const int m,
         const Boolean dftmode, const Boolean compmode);
DLLEXPORT int dct_create_table_fft(const int nSize);
DLLEXPORT int dct_create_table(const int nSize);
DLLEXPORT int dct_based_on_fft(double *pReal, double *pImag, const double *pInReal,
                     const double *pInImag);
DLLEXPORT int dct_based_on_dft(double *pReal, double *pImag, const double *pInReal,
                     const double *pInImag);
DLLEXPORT double df2(const double x, const double sf, const double f0p, const double wbp,
           const double f0z, const double wbz, const int fp, const int fz,
           double *buf, int *bufp);
DLLEXPORT double dfs(double x, double *a, int m, double *b, int n, double *buf,
           int *bufp);
DLLEXPORT int fft(double *x, double *y, const int m);
DLLEXPORT int fft2(double x[], double y[], const int n);
DLLEXPORT void fftcep(double *sp, const int flng, double *c, const int m, int itr,
            double ac);
DLLEXPORT int fftr(double *x, double *y, const int m);
DLLEXPORT int fftr2(double x[], double y[], const int n);
DLLEXPORT void freqt(double *c1, const int m1, double *c2, const int m2, const double a);
DLLEXPORT void gc2gc(double *c1, const int m1, const double g1, double *c2, const int m2,
           const double g2);
DLLEXPORT int gcep(double *xw, const int flng, double *gc, const int m, const double g,
         const int itr1, const int itr2, const double d, const int etype,
         const double e, const double f, const int itype);
DLLEXPORT double glsadf(double x, double *c, const int m, const int n, double *d);
DLLEXPORT double glsadf1(double x, double *c, const int m, const int n, double *d);
DLLEXPORT double glsadft(double x, double *c, const int m, const int n, double *d);
DLLEXPORT double glsadf1t(double x, double *c, const int m, const int n, double *d);
DLLEXPORT double cal_gconst(double *var, const int D);
DLLEXPORT double cal_gconstf(double **var, const int D);
DLLEXPORT double log_wgd(const GMM * gmm, const int m, const int L1, const int L2,
               const double *dat);
DLLEXPORT double log_add(double logx, double logy);
DLLEXPORT double log_outp(const GMM * gmm, const int L1, const int L2, const double *dat);
DLLEXPORT void fillz_GMM(GMM * gmm);
DLLEXPORT int alloc_GMM(GMM * gmm, const int M, const int L, const Boolean full);
DLLEXPORT int load_GMM(GMM * gmm, FILE * fp);
DLLEXPORT int save_GMM(const GMM * gmm, FILE * fp);
DLLEXPORT int free_GMM(GMM * gmm);
DLLEXPORT int prepareCovInv_GMM(GMM * gmm);
DLLEXPORT int prepareGconst_GMM(GMM * gmm);
DLLEXPORT int floorWeight_GMM(GMM * gmm, double floor);
DLLEXPORT int floorVar_GMM(GMM * gmm, double floor);
DLLEXPORT void gnorm(double *c1, double *c2, int m, const double g);
DLLEXPORT void grpdelay(double *x, double *gd, const int size, const int is_arma);
DLLEXPORT int histogram(double *x, const int size, const double min, const double max,
              const double step, double *h);
DLLEXPORT int ifft(double *x, double *y, const int m);
DLLEXPORT int ifft2(double x[], double y[], const int n);
DLLEXPORT double iglsadf(double x, double *c, const int m, const int n, double *d);
DLLEXPORT double iglsadf1(double x, double *c, const int m, const int n, double *d);
DLLEXPORT double iglsadft(double x, double *c, const int m, const int n, double *d);
DLLEXPORT double iglsadf1t(double x, double *c, const int m, const int n, double *d);
DLLEXPORT void ignorm(double *c1, double *c2, int m, const double g);
DLLEXPORT double imglsadf(double x, double *b, const int m, const double a, const int n,
                double *d);
DLLEXPORT double imglsadf1(double x, double *b, const int m, const double a, const int n,
                 double *d);
DLLEXPORT double imglsadft(double x, double *b, const int m, const double a, const int n,
                 double *d);
DLLEXPORT double imglsadf1t(double x, double *b, const int m, const double a, const int n,
                  double *d);
DLLEXPORT void imsvq(int *index, double *cb, const int l, int *cbsize, const int stage,
           double *x);
DLLEXPORT void ivq(const int index, double *cb, const int l, double *x);
DLLEXPORT void lbg(double *x, const int l, const int tnum, double *icb, int icbsize,
         double *cb, const int ecbsize, const int iter, const int mintnum,
         const int seed, const int centup, const double delta,
         const double end);
DLLEXPORT int levdur(double *r, double *a, const int m, double eps);
DLLEXPORT double lmadf(double x, double *c, const int m, const int pd, double *d);
DLLEXPORT double cascade_lmadf(double x, double *c, const int m, const int pd, double *d,
                     const int block_num, int *block_size);
DLLEXPORT double lmadft(double x, double *c, const int m, const int pd, double *d,
              const int block_num, int *block_size);
DLLEXPORT double lmadf1(double x, double *c, const int m, double *d, const int pd,
              const int m1, const int m2);
DLLEXPORT double lmadf2t(double x, double *b, const int m, const int pd, double *d,
               const int m1, const int m2);
DLLEXPORT int lpc(double *x, const int flng, double *a, const int m, const double f);
DLLEXPORT void lpc2c(double *a, int m1, double *c, const int m2);
DLLEXPORT int lpc2lsp(double *lpc, double *lsp, const int order, const int numsp,
            const int maxitr, const double eps);
DLLEXPORT int lpc2par(double *a, double *k, const int m);
DLLEXPORT void lsp2lpc(double *lsp, double *a, const int m);
DLLEXPORT void lsp2sp(double *lsp, const int m, double *x, const int l, const int gain);
DLLEXPORT int lspcheck(double *lsp, const int ord);
DLLEXPORT double lspdf_even(double x, double *f, const int m, double *d);
DLLEXPORT double lspdf_odd(double x, double *f, const int m, double *d);
DLLEXPORT double ltcdf(double x, double *k, int m, double *d);
DLLEXPORT void mc2b(double *mc, double *b, int m, const double a);
DLLEXPORT int mcep(double *xw, const int flng, double *mc, const int m, const double a,
         const int itr1, const int itr2, const double dd, const int etype,
         const double e, const double f, const int itype);
DLLEXPORT void mfcc(double *in, double *mc, const double sampleFreq, const double alpha,
          const double eps, const int wlng, const int flng, const int m,
          const int n, const int ceplift, const Boolean dftmode,
          const Boolean usehamming);
DLLEXPORT void maskCov_GMM(GMM * gmm, const int *dim_list, const int cov_dim,
                 const Boolean block_full, const Boolean block_corr);
DLLEXPORT void frqtr(double *c1, int m1, double *c2, int m2, const double a);
DLLEXPORT void mgc2mgc(double *c1, const int m1, const double a1, const double g1,
             double *c2, const int m2, const double a2, const double g2);
DLLEXPORT void mgc2sp(double *mgc, const int m, const double a, const double g, double *x,
            double *y, const int flng);
DLLEXPORT void mgclsp2sp(double a, double g, double *lsp, const int m, double *x,
               const int l, const int gain);
DLLEXPORT int mgcep(double *xw, int flng, double *b, const int m, const double a,
          const double g, const int n, const int itr1, const int itr2,
          const double dd, const int etype, const double e, const double f,
          const int itype);
DLLEXPORT double newton(double *x, const int flng, double *c, const int m, const double a,
              const double g, const int n, const int j, const double f);
DLLEXPORT double mglsadf(double x, double *b, const int m, const double a, const int n,
               double *d);
DLLEXPORT double mglsadf1(double x, double *b, const int m, const double a, const int n,
                double *d);
DLLEXPORT double mglsadft(double x, double *b, const int m, const double a, const int n,
                double *d);
DLLEXPORT double mglsadf1t(double x, double *b, const int m, const double a, const int n,
                 double *d);
DLLEXPORT int str2darray(char *c, double **x);
DLLEXPORT int isfloat(char *c);
DLLEXPORT double mlsadf(double x, double *b, const int m, const double a, const int pd,
              double *d);
DLLEXPORT double mlsadft(double x, double *b, const int m, const double a, const int pd,
               double *d);
DLLEXPORT void msvq(double *x, double *cb, const int l, int *cbsize, const int stage,
          int *index);
DLLEXPORT void ndps2c(double *n, const int l, double *c, const int m);
DLLEXPORT void norm0(double *x, double *y, int m);
DLLEXPORT int nrand(double *p, const int leng, const int seed);
DLLEXPORT double nrandom(unsigned long *next);
DLLEXPORT unsigned long srnd(const unsigned int seed);
DLLEXPORT void par2lpc(double *k, double *a, const int m);
DLLEXPORT void phase(double *p, const int mp, double *z, const int mz, double *ph,
           const int flng, const int unlap);
DLLEXPORT double poledf(double x, double *a, int m, double *d);
DLLEXPORT double poledft(double x, double *a, int m, double *d);
DLLEXPORT void reverse(double *x, const int l);
DLLEXPORT double rmse(double *x, double *y, const int n);
DLLEXPORT void output_root_pol(Complex * x, int odr, int form);
DLLEXPORT void root_pol(double *a, const int odr, Complex * x, const int a_zero,
              const double eps, const int itrat);
DLLEXPORT Complex *cplx_getmem(const int leng);
DLLEXPORT int smcep(double *xw, const int flng, double *mc, const int m, const int fftsz,
          const double a, const double t, const int itr1, const int itr2,
          const double dd, const int etype, const double e, const double f,
          const int itype);
DLLEXPORT int uels(double *xw, const int flng, double *c, const int m, const int itr1,
         const int itr2, const double dd, const int etype, const double e,
         const int itype);
DLLEXPORT double ulaw_c(const double x, const double max, const double mu);
DLLEXPORT double ulaw_d(const double x, const double max, const double mu);
DLLEXPORT int vc(const GMM * gmm, const DELTAWINDOW * window, const size_t total_frame,
       const size_t source_vlen, const size_t target_vlen,
       const double *gv_mean, const double *gv_vari,
       const double *source, double *target);
DLLEXPORT int vq(double *x, double *cb, const int l, const int cbsize);
DLLEXPORT double edist(double *x, double *y, const int m);
DLLEXPORT double window(Window type, double *x, const int size, const int nflg);
DLLEXPORT double zcross(double *x, const int fl, const int n);
DLLEXPORT double zerodf(double x, double *b, int m, double *d);
DLLEXPORT double zerodft(double x, double *b, const int m, double *d);
DLLEXPORT double zerodf1(double x, double *b, int m, double *d);
DLLEXPORT double zerodf1t(double x, double *b, const int m, double *d);

/* wavsplit and wavjoin */
DLLEXPORT void copy_wav_header(Wavfile * dest_wav, const Wavfile * source_wav);
DLLEXPORT void separate_path(char **dir, char **name, char *path);
DLLEXPORT Boolean get_wav_list(Filelist * filelist, const char *dirname);
DLLEXPORT Boolean wavread(Wavfile * wavfile, const char *fullpath);
DLLEXPORT Boolean wavwrite(Wavfile * wavfile, const char *outpath);
DLLEXPORT void wavsplit(Wavfile * wavout, const Wavfile * wavin);
DLLEXPORT void free_wav_list(Filelist * filelist);
DLLEXPORT void free_wav_data(Wavfile * wavfile);
DLLEXPORT void wavjoin(Wavfile * wavout, const Wavfile * wavin);
DLLEXPORT int search_wav_list(Filelist * filelist, char *key);

/* excitation */
DLLEXPORT void excite(double *pitch, int n, double *out, int fprd, int iprd, Boolean gauss, int seed_i);

DLLEXPORT void swipe(double *input, double *output, int length, int samplerate, int frame_shift, double min, double max, double st, int otype);

/****************************************************************
    The RAPT pitch tracker

        return   value :    0 -> completed normally
                            1 -> invalid/inconsistent parameters
                            2 -> input range too small
                            3 -> problem in init_dp_f0

*****************************************************************/
DLLEXPORT int rapt(float *input, float* output, int length, double sample_freq,
     int frame_shift, double minF0, double maxF0, double voice_bias, int otype);

DLLEXPORT void b2c(double *b, int m1, double *c, int m2, double a);


#endif  /* SPTK_H_ */
