%module sptk
%{
#define SWIG_FILE_WITH_INIT
#include "SPTK.h"
  double ave(double *array, int n) {  return 1.0; }
%}

%include "numpy.i"

%init %{
  import_array();
%}

// FFT
%apply (double *INPLACE_ARRAY1, int DIM1) {(double *x, int n), (double *y, int m)}
%rename (fft) my_fft;
%inline %{
  int my_fft(double *x, int n, double *y, int m) {
    if (n != m) {
      return 0;
    }
    return fft(x, y, m);
  }
%}

// MCEP
%apply (double* IN_ARRAY1, int DIM1) {(double *xw, int flng)}
%apply (double* ARGOUT_ARRAY1, int DIM1) {(double *mc, int m)}
%rename (mcep) my_mcep;
%inline %{
  int my_mcep(double *xw, const int flng, double *mc, const int m, const double a,
	      const int itr1, const int itr2, const double dd, const int etype,
	      const double e, const double f, const int itype) {
    // m-1 is the order of mel-cepstrum except for 0-th order coef.
    return mcep(xw, flng, mc, m-1, a, itr1, itr2, dd, etype, e, f, itype);
  }
%}


%include "SPTK.h"
