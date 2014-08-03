import sptk
import numpy as np
from pylab import *

print "peform gexp"
print sptk.gexp(1.0, 2.0)

x = np.arange(256, dtype=float)
y = np.arange(256, dtype=float)

#print sptk.mcep(x, 512, y, 20, 0.41, 1, 10, 0.001, 0, 0.0001, 0.0001, 0)
xx = np.random.randn(256) + 10.0
print xx.shape
ret, mc = sptk.mcep(xx, 30, 0.41, 2, 30, 0.001, 0, 0.0, 0.00001, 0)
if True:
    print mc
    print np.array(mc).shape
    plot(mc)
    show()
    quit()

print sptk.fft(x, y)
spec = sqrt(x*x+y*y)

plot(spec)
show()
