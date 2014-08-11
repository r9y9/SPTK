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
    """
    Generalized Exponential Function

    Parameters
    ----------
      r : float
          gamma
      x : float
          arg
    
    Return
    ------
    value
    
    """
    return csptk.gexp(r, x)

def glog(r, x):
    """
    Generalized Logarithmic Function
    
    Parameters
    ----------
      r : float
          gamma
      x : float
          arg
    
    Return
    ------
    value
    
    """
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

class MLSADF:
    """
    Mel-Log Spectrum Approximation (MLSA) Digital Filter

    Parameters
    ----------
    order : int
        order of input filter coefficients
    
    pd : int
        order of pade approximation (4 or 5)

    Attributes
    ----------
    pd : int
        order of pade approximation (4 or 5)

    order : int
        order of input filter coefficients

    delay : array, shape (`3*(pd+1)+pd*(order+2)`)
        filter delay used during filtering.
        See mlsadf.c for this magic shape.

    """
    def __init__(self, order, pd=4):
        assert pd == 4 or pd == 5

        self.pd = pd
        self.order = order
        self.__setup_delay(order, pd)

    def __setup_delay(self, order, pd):
        # see mlsadf.c for the following magic allocation
        self.delay = np.zeros(3*(pd+1)+pd*(order+2))
        
    def filter(self, x, b, alpha=0.41):
        """
        Generate one sample by filtering
        
        Parameters
        ----------
        x : float
             an excitation sample
        b : array, shape (`order + 1`)
             filter coefficients
        alpha : float
             all-pass constant

        Return
        ------
        filtered sample
        
        """
        return csptk.mlsadf(x, b, alpha, self.pd, self.delay)

class MLSASynthesizer:
    """
    MLSADF-based speech waveform synthesizer

    Parameters
    ----------
    order : int
        order of input filter coefficients
    
    pd : int
        order of pade approximation (4 or 5)

    Attributes
    ----------
    f : int
        mlsa digital filter that is used in synthesis

    """
    def __init__(self, order, pd=4):
        self.f = MLSADF(order=order, pd=pd)

    def synthesis_one_frame(self, excite, previous_mcep, current_mcep, 
                            alpha=0.41):
        """
        Synthesis one frame waveform
        
        Parameters
        ----------
        excite : array, shape(`frame_len`)
             excitation signal
        previous_mcep : array, shape (`order + 1`)
             mel-cepstrum at previous frame
        current_mcep : array, shape (`order + 1`)
             mel-cepstrum at current frame
        alpha : float
             all-pass constant

        Return
        ------
        synthesized waveform for one frame
        
        """
        previous_coef = mc2b(previous_mcep, alpha=alpha)
        current_coef = mc2b(current_mcep, alpha=alpha)

        slope = (current_coef - previous_coef)/float(len(excite))
        
        part_of_speech = np.zeros(excite.shape)
        interpolated_coef = previous_coef.copy()
        
        for i in range(len(excite)):
            scaled_excitation = excite[i] * np.exp(interpolated_coef[0])
            part_of_speech[i] = self.f.filter(scaled_excitation, 
                                              interpolated_coef)
            interpolated_coef += slope
        
        return part_of_speech

    def synthesis(self, excite, mcep_sequence, alpha=0.41, frame_shift=80):
        """
        Synthesis waveform
        
        Parameters
        ----------
        excite : array, shape(`frame_len`)
             excitation signal
        mcep_sequence : array, shape (`number of frames`, `order + 1`)
             mel-cepstrum sequence over time
        alpha : float
             all-pass constant
        frame_shift : int
             frame_shift
        
        Return
        ------
        synthesized waveform
        
        """
        synthesized = np.zeros(len(excite))

        previous_mcep = mcep_sequence[0]
        for i in range(len(mcep_sequence)):
            if i > 0:
                previous_mcep = mcep_sequence[i-1]
            current_mcep = mcep_sequence[i]

            s, e = i*frame_shift, (i+1)*frame_shift
            if e >= len(excite):
                break

            part_of_speech = self.synthesis_one_frame(excite[s:e],
                                                      previous_mcep,
                                                      current_mcep,
                                                      alpha=alpha)
            synthesized[s:e] = part_of_speech
            
        return synthesized

def mfcc(x, order=12, samplerate=16000.0, alpha=0.97, eps=1.0, 
         num_filterbank=20, ceplift=22,
         dftmode=False, usehamming=False, czero=False, power=False):
    """
    mfcc computes Mel-Frequency Cepstrum Coefficients

    Parameters
    ----------
      x : array, shape (`frame_len`)
           a input signal
      order : int
           order of MFCC that will be extracted
      samplerate : float
           sampling frequency
      alpha : float
           pre-emphasis coefficients (this doesn't mean all-pass constant 
           in mel-cepstrum analysis)
      eps : float
           epsiron
      num_filterbank : int
           number of mel-filterbanks
      cepslift : int
           number of cepstrum liftering
      dftmode : bool
           whether uses dft or not
      usehamming : bool
           whether uses hamming window or not
      czero : nool
           whether return c0 static MFCC features as a returned joint vector
      power : nool
           whether return power as a returned  joint vector

    Return
    ------
    MFCC

    """
    assert order+1 <= num_filterbank

    # order of MFCC + 0-th + power
    cc = np.zeros(order+2)

    csptk.mfcc(x, cc, samplerate, alpha, eps, 
               num_filterbank, ceplift, dftmode, usehamming)

    # after ccall we get 
    # mfcc[0], mfcc[1], mfcc[2], ... mfcc[m-1], E(C0), Power
    
    if not czero and power:
        cc[-2] = cc[-1]

    if not power:
        cc = cc[:-1]

    if not czero:
        cc = cc[:-1]
    
    return cc
