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
  (double *x, int n), (double *y, int m)
}

// MCEP
%apply (double* IN_ARRAY1, int DIM1) {(double *xw, int flng)}
%apply (double* ARGOUT_ARRAY1, int DIM1) {(double *mc, int m)}

// SWIPE
%apply (double* IN_ARRAY1, int DIM1) {(double *input, int len1)}
%apply (double* INPLACE_ARRAY1, int DIM1) {(double *output, int len2)}

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

%rename (fft) my_fft;
%inline %{
  int my_fft(double *x, int n, double *y, int m) {
    if (n != m) {
      return 0;
    }
    return fft(x, y, m);
  }
%}

%rename (mcep) my_mcep;
%inline %{
  int my_mcep(double *xw, const int flng, double *mc, 
	      const int m, const double a,
	      const int itr1, const int itr2,
	      const double dd, const int etype,
	      const double e, const double f, const int itype) {
    // m-1 is the order of mel-cepstrum except for 0-th order coef.
    return mcep(xw, flng, mc, m-1, a, itr1, itr2, dd, etype, e, f, itype);
  }
%}

%rename (swipe) my_swipe;
%inline %{
  void my_swipe(double *input, int len1, double *output, int len2, 
		int samplerate, int frame_shift, 
		double min, double max, double st, int otype) {
    int expected_output_len = len1/frame_shift+1;
    if (expected_output_len != len2) {
      // TODO
    }
    swipe(input, output, len1, samplerate, frame_shift, min, max, st, otype);
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

%include "SPTK.h"
