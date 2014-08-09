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
    mel-cepstrum : array, shape (`order + 1`)

    """
    mc = np.zeros(order+1)
    end_condition = csptk.mcep(x, mc, alpha, iter1, iter2,
                               dd, etype, e, f, itype, 0)
    # ignore end_condition
    return mc

# will be deprecated
def fft(x, y):
    csptk.fft(x, y)

def gexp(r, x):
    return csptk.gexp(r, x)

def glog(r, x):
    return csptk.glog(r, x)

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

def freqt(c, order, alpha):
    """
    freqt peforms frequency transform on cepstrum. It is used 
    to transform linear frequency to mel frequency domain on 
    cepstrum.

    Parameters
    ----------
      c : array, shape (`order of cepstrum ` + 1)
          cepstrum
      order : int
          desired order of cepstrum that will be converted  
      alpha : float
          all-pass constant

    Return
    ------
    mel-cepstrum : array, shape(=b)

    """
    m2 = order+1
    convertedC = np.zeros(m2)
    csptk.freqt(c, convertedC, alpha)
    return convertedC

def gnorm(c, gamma):
    """
    gnorm performs gain normalization on generalized cepstrum

    Parameters
    ----------
      c : array, shape (`order of cepstrum` + 1)
          generalized cepstrum
      gamma : float
          TODO

    Return
    ------
    normalized generalized cepstrum : array, shape(=c)
    
    """
    c2 = np.zeros(c.shape)
    csptk.gnorm(c, c2, gamma)
    return c2

def ignorm(c, gamma):
    """
    gnorm performs inverse gain normalization on generalized cepstrum

    Parameters
    ----------
      c : array, shape (`order of cepstrum` + 1)
          generalized cepstrum
      gamma : float
          TODO

    Return
    ------
    generalized cepstrum : array, shape(=c)
    
    """
    c2 = np.zeros(c.shape)
    csptk.ignorm(c, c2, gamma)
    return c2


def mgcep(x, order=20, alpha=0.41, gamma=0.0, n=None,
          iter1=2, iter2=30, 
          dd=0.001, etype=0, e=0.0, f=0.0001, itype=0, otype=0):
    """
    Mel Generalized Cepstrum analysis
    
    Parameters
    ----------
      x : array, shape (`frame_len`)
          A input frame
      order : int
          order of mel generalized cepstrum that will be extracted
      alpha : float
          all pass constant
      n : int (default=len(x)-1)
          order of recursions
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
      otype : int
          output data type
              (0) mel generalized cepstrum: (c~0...c~m)
              (1) MGLSA filter coefficients: b0...bm
              (2) K~,c~'1...c~'m
              (3) K,b'1...b'm
              (4) K~,g*c~'1...g*c~'m
              (5) K,g*b'1...g*b'm

    Return
    ------
    mel generalized cepstrum : array, shape (`order + 1`)

    """
    mgc = np.zeros(order+1)

    if n == None:
        n = len(x)-1

    end_condition = csptk.mgcep(x, mgc, alpha, gamma, n, 
                                iter1, iter2, 
                                dd, etype, e, f, itype)

    if otype == 0 or otype == 1 or otype == 2 or otype == 4:
        csptk.ignorm(mgc, mgc, gamma)
    
    if otype == 0 or otype == 2 or otype == 4:
        csptk.b2mc(mgc, mgc, alpha)

    if otype == 2 or otype == 4:
        csptk.gnorm(mgc, mgc, gamma)

    if otype == 4 or otype == 5:
        mgc[1:] *= gamma
    
    return mgc

def blackman(x, normalize=0):
    """
    blackman window

    Parameters
    ----------
      x : array, shape (`frame len`)
           input frame
      normalize : int
           0 : don't normalize
           1 : normalize by power
           2 : normalize by magnitude

    Return
    ------
    windowed signal

    """
    return __window(0, x, normalize)

def hamming(x, normalize=0):
    """
    hamming window

    Parameters
    ----------
      x : array, shape (`frame len`)
           input frame
      normalize : int
           0 : don't normalize
           1 : normalize by power
           2 : normalize by magnitude

    Return
    ------
    windowed signal

    """
    return __window(1, x, normalize)

def hanning(x, normalize=0):
    """
    hanning window

    Parameters
    ----------
      x : array, shape (`frame len`)
           input frame
      normalize : int
           0 : don't normalize
           1 : normalize by power
           2 : normalize by magnitude

    Return
    ------
    windowed signal

    """
    return __window(2, x, normalize)

def barlett(x, normalize=0):
    """
    barlett window

    Parameters
    ----------
      x : array, shape (`frame len`)
           input frame
      normalize : int
           0 : don't normalize
           1 : normalize by power
           2 : normalize by magnitude

    Return
    ------
    windowed signal

    """
    return __window(3, x, normalize)

def trapezoid(x, normalize=0):
    """
    trapezoid window

    Parameters
    ----------
      x : array, shape (`frame len`)
           input frame
      normalize : int
           0 : don't normalize
           1 : normalize by power
           2 : normalize by magnitude

    Return
    ------
    windowed signal

    """
    return __window(4, x, normalize)

def rectangular(x, normalize=0):
    """
    rectangular window

    Parameters
    ----------
      x : array, shape (`frame len`)
           input frame
      normalize : int
           0 : don't normalize
           1 : normalize by power
           2 : normalize by magnitude

    Return
    ------
    windowed signal

    """
    return __window(4, x, normalize)

def __window(t, x, normalize=0):
    assert len(x) > 0
    # must be float type
    y = np.array(x.copy(), dtype=float)
    # do we need gain as a return value?
    gain = csptk.window(t, y, normalize)
    return y

def c2ir(c, length):
    """
    cepstrum to impulse response conversion

    Parameters
    ----------
      c : array, shape (`order + 1`)
           cepstrum
      length : int
           length of impulse response

    Return
    ------
    impulse response

    """    
    h = np.zeros(length)
    dummy = 0
    csptk.c2ir(c, h, dummy)
    return h
