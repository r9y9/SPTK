# SPTK
-----------------

This repository hosts a modified version of [Speech Signal Processing Toolkit (SPTK)](http://sp-tk.sourceforge.net/) to use the SPTK as **API** (not command line program) from external programs. For command line use, please check [the original one](http://sp-tk.sourceforge.net/).

Changes from the original SPTK are summerized as follows:

- Integrate [waf]((https://code.google.com/p/waf/)) build tool
- Add python wrapper using SWIG (experimental)

## Install

    ./waf configure
    ./waf
    sudo ./waf install
    
## Getting Started

### C

    gcc call_sptk.c `pkg-config SPTK --cflags --libs`
    
### Go

Go wrapper can be found at [https://github.com/r9y9/gossp/tree/master/3rdparty/sptk](https://github.com/r9y9/gossp/tree/master/3rdparty/sptk).

### Python [experimental]

It is assumed that swig is installed.

    ./waf configure --python
    ./waf
    sudo ./waf install

#### SWIPE f0 estimation example from python

```python
import sptk
import numpy as np
import scipy.io.wavfile as wavfile
from pylab import plot, show, legend, xlabel, ylabel

fs, data = wavfile.read("test.wav")

f0 = sptk.swipe(data, samplerate=fs, frame_shift=80)

axis = [float(n)*80/fs for n in range(len(f0))]
xlabel("Time (sec)")
ylabel("Hz")
plot(axis, f0, label="F0 estimation result using SWIPE")
legend()
show()
```
	 
## License

[MIT](./LICENSE)

## Author

[Ryuichi YAMAMOTO](https://github.com/r9y9)