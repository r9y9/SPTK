#!/usr/bin/python
# coding: utf-8

import csptk

import numpy as np

def mcep(x, order=20, alpha=0.41, iter1=2, iter2=30, 
         dd=0.001, etype=0, e=0.0, f=0.0001, itype=0):
    """
    Mel-Cepstrum analysis
    
    Parameters
    ----------
      x : array, shape (`frame_len`)
          A input frame
      order : int
          order of mel-cepstrum that will be extracted
      alpha : float
          all pass constant
      iter1 : int
          minimum number of iteration
      iter2 : int
          maximum number of iteration
      dd : float
          threshold
      etype : int
          type of paramter `e` 
               (0) not used
               (1) initial value of log-periodogram
               (2) floor of periodogram in db
      e : float
          initial value for log-periodogram or floor of periodogram in db
      f : float
          mimimum value of the determinant of normal matrix
      itype : float
          input data type:
              (0) windowed signal
              (1) log amplitude in db
              (2) log amplitude
              (3) amplitude
              (4) periodogram

    Return
    ------
    mel-cepstrum : array, shape (`order`)

    """
    end_condition, mc = csptk.mcep(x, order, alpha, iter1, iter2,
                                   dd, etype, e, f, itype)
    # ignore end_condition
    return mc

# will be deprecated
def fft(x, y):
    csptk.fft(x, y)

# will be deprecated
def gexp(r, x):
    return csptk.gexp(r, x)

def swipe(x, samplerate, frame_shift, 
          min=50.0, max=800.0, st=0.3, otype=1):
    """
    SWIPE - A Saw-tooth Waveform Inspired Pitch Estimation

    Parameters
    ----------
      x : array, shape (`the number of audio samples`)
          A whole signal
      samplerate : int
          samplerate
      frame_shift : int
          frame_shift
      min : float (default=50.0)
          minimum fundamental frequency
      max : float (default=800.0)
          maximum fundamental frequency
      st : float (default=0.3)
          voice/unvoiced threthold 
      otype : int (default=1)
          output format (0) pitch (1) f0 (2) log(f0)

    Return
    ------
    f0 sequence : array, shape(`len(x)/frame_shift+1`)

    """
    expected_len = len(x)/frame_shift + 1
    f0 = np.zeros(expected_len, dtype=float)
    csptk.swipe(x, f0, samplerate, frame_shift, min, max, st, otype)
    return f0

def mc2b(mc, alpha):
    """
    mc2b converts mel-cepsrum to MLSA filter coefficients.

    Parameters
    ----------
      mc : array, shape (`order of mel-cepstrum` + 1)
           mel-cepstrum
      alpha : float
           all-pass constant

    Return
    ------
    MLSA filter coefficients : array, shape(=mc)

    """
    b = np.zeros(mc.shape)
    csptk.mc2b(mc, b, alpha)
    return b

def b2mc(b, alpha):
    """
    b2mc converts MLSA filter coefficients to mel-cepstrum

    Parameters
    ----------
      b : array, shape (`number of MLSA filter coefficients`)
           MLSA filter coefficients
      alpha : float
           all-pass constant

    Return
    ------
    mel-cepstrum : array, shape(=b)

    """
    mc = np.zeros(b.shape)
    csptk.b2mc(b, mc, alpha)
    return mc
