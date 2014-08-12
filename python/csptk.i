%module csptk
%{
#define SWIG_FILE_WITH_INIT
#include "SPTK.h"
%}

%include "numpy.i"

%init %{
  import_array();
%}

// FFT
%apply (double *INPLACE_ARRAY1, int DIM1)
{
  (double *x_fft, int n_fft),
  (double *y_fft, int m_fft)
}

// MCEP
%apply (double* IN_ARRAY1, int DIM1) {
  (double *xw_mcep, int flng_mcep),
  (double *mc_mcep, int m_mcep)
}

// SWIPE
%apply (double* IN_ARRAY1, int DIM1) {
  (double *input_swipe, int len1_swipe)
}
%apply (double* INPLACE_ARRAY1, int DIM1) {
  (double *output_swipe, int len2_swipe)
}

// MC2B
%apply (double *IN_ARRAY1, int DIM1)
{
  (double *mc_mc2b, int m_mc2b),
  (double *b_mc2b, int n_mc2b)
}

// B2MC 
%apply (double *IN_ARRAY1, int DIM1)
{
  (double *b_b2mc, int n_b2mc),
  (double *mc_b2mc, int m_b2mc)
}

// freqt 
%apply (double *INPLACE_ARRAY1, int DIM1)
{
  (double *c1_freqt, int m1_freqt),
  (double *c2_freqt, int m2_freqt)
}

// gnorm
%apply (double *INPLACE_ARRAY1, int DIM1)
{
  (double *c1_gnorm, int n_gnorm),
  (double *c2_gnorm, int m_gnorm)
}

// ignorm
%apply (double *INPLACE_ARRAY1, int DIM1)
{
  (double *c1_ignorm, int n_ignorm),
  (double *c2_ignorm, int m_ignorm)
}

// mgcep
// TODO: why not work with INPLACE_ARRAY1?
%apply (double *IN_ARRAY1, int DIM1)
{
  (double *xw_mgcep, int flng_mgcep),
  (double *b_mgcep, int m_mgcep)
}

// window
%apply (double *IN_ARRAY1, int DIM1) {
  (double *x_window, int size_window)
}

// c2ir
%apply (double *IN_ARRAY1, int DIM1)
{
  (double *c_c2ir, const int nc_c2ir), 
  (double *h_c2ir, const int leng_c2ir)
}

// mlsadf
%apply (double *IN_ARRAY1, int DIM1) 
{
  (double *b_mlsadf, const int m_mlsadf),
  (double *d_mlsadf, int length_of_total_delay_mlsadf)
}

// mfcc
%apply (double *IN_ARRAY1, int DIM1) 
{
  (double *in_mfcc, int flen_mfcc), 
  (double *mc_mfcc, int m_mfcc)
}

%rename (fft) my_fft;
%inline %{
  int my_fft(double *x_fft, int n_fft,
	     double *y_fft, int m_fft) {
    if (n_fft != m_fft) {
      return 0;
    }
    return fft(x_fft, y_fft, m_fft);
  }
%}

// TODO remove dummy variable
%rename (mcep) my_mcep;
%inline %{
  int my_mcep(double *xw_mcep, const int flng_mcep,
	      double *mc_mcep, const int m_mcep,
	      const double a,
	      const int itr1, const int itr2,
	      const double dd, const int etype,
	      const double e, const double f, const int itype, int dummy) {
    // m-1 is the order of mel-cepstrum except for 0-th order coef.
    return mcep(xw_mcep, flng_mcep, mc_mcep, m_mcep-1,
		a, itr1, itr2, dd, etype, e, f, itype);
  }
%}

%rename (swipe) my_swipe;
%inline %{
  void my_swipe(double *input_swipe, int len1_swipe,
		double *output_swipe, int len2_swipe, 
		int samplerate, int frame_shift, 
		double min, double max, double st, int otype) {
    int expected_output_len = len1_swipe/frame_shift+1;
    if (expected_output_len != len2_swipe) {
      // TODO
    }
    swipe(input_swipe, output_swipe, len1_swipe,
	  samplerate, frame_shift, min, max, st, otype);
  }
%}

%rename (mc2b) my_mc2b;
%inline %{
  void my_mc2b(double *mc_mc2b, int m_mc2b,
	       double *b_mc2b, int n_mc2b, const double a) {
    int order = m_mc2b - 1; // except 0th order
    mc2b(mc_mc2b, b_mc2b, order, a);
  }
%}

%rename (b2mc) my_b2mc;
%inline %{
  void my_b2mc(double *b_b2mc, int n_b2mc, 
	       double *mc_b2mc, int m_b2mc, const double a) {
    int order = m_b2mc - 1; // except 0th order
    b2mc(b_b2mc, mc_b2mc, order, a);
  }
%}

%rename (freqt) my_freqt;
%inline %{
  void my_freqt(double *c1_freqt, int m1_freqt,
		double *c2_freqt, int m2_freqt, const double a) {
    freqt(c1_freqt, m1_freqt-1, c2_freqt, m2_freqt-1, a);
  }
%}

%rename (gnorm) my_gnorm;
%inline %{
  void my_gnorm(double *c1_gnorm, int n_gnorm,
	     double *c2_gnorm, int m_gnorm, const double g) {
    int order = n_gnorm - 1;
    gnorm(c1_gnorm, c2_gnorm, order, g);
  }
%}

%rename (ignorm) my_ignorm;
%inline %{
  void my_ignorm(double *c1_ignorm, int n_ignorm,
		 double *c2_ignorm, int m_ignorm, const double g) {
    int order = n_ignorm - 1;
    ignorm(c1_ignorm, c2_ignorm, order, g);
  }
%}

%rename (mgcep) my_mgcep;
%inline %{
  int my_mgcep(double *xw_mgcep, int flng_mgcep,
	       double *b_mgcep, int m_mgcep,
	       const double a,
	       const double g, const int n, const int itr1, const int itr2,
	       const double dd, const int etype, 
	       const double e, const double f,
	       const int itype) {
    return mgcep(xw_mgcep, flng_mgcep,
		 b_mgcep, m_mgcep-1,
		 a,
		 g, n, itr1, itr2, 
		 dd, etype,
		 e, f, 
		 itype);
  }
%}

%rename (window) my_window;
%inline %{
  double my_window(int type, 
		 double *x_window, const int size_window,
		 const int nflg) {
    return window(type, x_window, size_window, nflg);
  }
%}

%rename (c2ir) my_c2ir;
%inline %{
  void my_c2ir(double *c_c2ir, const int nc_c2ir, 
	       double *h_c2ir, const int leng_c2ir, int dummy) {
    c2ir(c_c2ir, nc_c2ir, h_c2ir, leng_c2ir);
  }
%}

%rename (mlsadf) my_mlsadf;
%inline %{
  double my_mlsadf(double x, 
		double *b_mlsadf, const int m_mlsadf,
		const double a, const int pd,
		double *d_mlsadf, int length_of_total_delay_mlsadf) {
    // length_of_total_delay_mlsadf is a dummy variable 
    return mlsadf(x, b_mlsadf, m_mlsadf-1, a, pd, d_mlsadf);
  }
%}

%rename (mfcc) my_mfcc;
%inline %{
  void my_mfcc(double *in_mfcc, int flen_mfcc, 
	       double *mc_mfcc, int m_mfcc, 
	       const double sampleFreq, const double alpha, const double eps,
	       const int n, const int ceplift, const int dftmode,
	       const int usehamming) {
    mfcc(in_mfcc, mc_mfcc, sampleFreq, alpha, eps, flen_mfcc, flen_mfcc, 
	 m_mfcc-1, n, ceplift, dftmode, usehamming);
  }
%}

%include "SPTK.h"
