import sptk
import numpy as np
from pylab import *
import scipy.io.wavfile

print "peform gexp"
print sptk.gexp(1.0, 2.0)

fs, data = scipy.io.wavfile.read("test.wav")
print data.shape
data = np.array(data, dtype=float)

x = data[2000:2000+256]
y = data[2000:2000+256]

xx = x
print xx.shape
ret, mc = sptk.mcep(xx, 30, 0.41, 2, 30, 0.001, 0, 0.0, 0.00001, 0)
if True:
    plot(mc, label="mcep")
    legend()
    show()
    quit()

print sptk.fft(x, y)
spec = sqrt(x*x+y*y)

f0 = np.zeros(data.shape[0]/80+1, dtype=float64)
sptk.swipe(data, f0, fs, 80, 20.0, 800.0, 0.3, 1)

plot(f0)
show()

plot(spec)
show()
