import csptk

import numpy as np

def mcep(x, order=20, alpha=0.41, iter1=2, iter2=30, 
         dd=0.001, etype=0, e=0.0, f=0.0001, itype=0):
    """
    mcep perform mel-cepstrum analysis.
    
    Parameters
    ----------
      x : array, shape (`frame_len`)
          A input frame
      order : int
          order of mel-cepstrum that will be extracted
      alpha : float
          all pass constant

    Return
    ------
    mel-cepstrum

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
      max : int (default=800.0)
          maximum fundamental frequency
      st : float (default=0.3)
          voice/unvoiced threthold 
      otype : int (default=1)
          output format (0) pitch (1) f0 (2) log(f0)

    Return
    ------
    f0 sequence

    """
    expected_len = len(x)/frame_shift + 1
    f0 = np.zeros(expected_len, dtype=float)
    csptk.swipe(x, f0, samplerate, frame_shift, min, max, st, otype)
    return f0
