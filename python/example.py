#!/usr/bin/python
# coding: utf-8

import sptk
import numpy as np
from pylab import plot, show, legend
import scipy.io.wavfile

def sptklib_call():
    print sptk.gexp(1, 3)

def sptk_call():
    fs, data = scipy.io.wavfile.read("test.wav")
    data = np.array(data)

    # create slice for testing
    x = data[2000:2000+256]

    # Mel-Cepstrum
    mc = sptk.mcep(x, order=40)
    plot(mc, label="mcep")
    legend()
    show()
    
    # F0 estimation
    f0 = sptk.swipe(data, samplerate=fs, frame_shift=80)
    plot(f0, label="f0")
    show()
    
if __name__=="__main__":
    sptklib_call()
    sptk_call()
