# SPTK
-----------------

This repository hosts a modified version of [Speech Signal Processing Toolkit (SPTK)](http://sp-tk.sourceforge.net/) to use the SPTK as **API** (not command line program) from external programs. For command line use, please check [the original one](http://sp-tk.sourceforge.net/).

Changes from the original SPTK are summerized as follows:

- Integrate [waf](https://code.google.com/p/waf/) build tool
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

#### Tutorial

[SPTK を Pythonから呼ぶ | nbviewer](http://nbviewer.ipython.org/github/r9y9/SPTK/blob/master/notebook/SPTK%20calling%20from%20python.ipynb)
	 
## License

- Original SPTK - [BSD](./COPYING)
- Python wrapper - [MIT](./LICENSE)

## Author

[Ryuichi YAMAMOTO](https://github.com/r9y9)