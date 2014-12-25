import sptk
import scipy.io.wavfile as wavfile
from pylab import plot, show, legend
import numpy as np

fs, data = wavfile.read("test.wav")

f0 = sptk.swipe(data, samplerate=fs, frame_shift=80)
plot(f0)
show()

# test_data = np.ones(1024)
test_data = data[5000:6024+1024]#/32768.0
plot(test_data, "r+", label="original")
d = sptk.blackman(test_data)
print d - test_data
plot(d, label="windowed")
legend()
show()
# quit()

cc = sptk.mfcc(test_data, order=12)
print cc
plot(cc, label="mfcc")
legend()
show()

d = data[1000:2024]
#mgc = sptk.mgcep(d, order=20, alpha=0.41, gamma=0.0)
print d
mc = sptk.mcep(d, order=20, alpha=0.41)
print d
#plot(mgc, label="mgcep")
plot(mc, label="mcep")
#print mgc-mc
legend()
show()
# quit()

mc = sptk.mcep(data[1000:2024], order=20, alpha=0.41)
b = sptk.mc2b(mc, alpha=0.41)
plot(mc, label="mcep")
plot(b, label="b")
reconstructed = sptk.b2mc(b, alpha=0.41)
plot(reconstructed, label="reconstructed")

print (mc-reconstructed)

legend()
show()

# back to linear frequency
convertedC = sptk.freqt(mc, order=20, alpha=-0.41)
plot(mc, label="mcep")
plot(convertedC, label="converted cepstrum")
legend()
show()

mc1 = sptk.gnorm(convertedC, gamma=0.5)
mc2 = sptk.ignorm(mc1, gamma=0.5)
plot(convertedC, label="converted cepstrum")
plot(mc1, label="mc1")
plot(mc2, label="mc2")
print convertedC - mc2
legend()
show()
