# SPTK

[![Build Status](https://travis-ci.org/r9y9/SPTK.svg?branch=master)](https://travis-ci.org/r9y9/SPTK)
[![Build status](https://ci.appveyor.com/api/projects/status/8y2w0gbvve6anrsn/branch/master?svg=true)](https://ci.appveyor.com/project/r9y9/sptk/branch/master)

This repository hosts a modified version of [Speech Signal Processing Toolkit (SPTK)](http://sp-tk.sourceforge.net/) for embedded programs. For command line use, please check [the original SPTK](http://sp-tk.sourceforge.net/) instead.

The important changes from the original SPTK are summerized as follows:

- Integrate [waf](https://code.google.com/p/waf/) build tool
 - `./waf configure && ./waf` compiles all sources in `bin` and `lib` directories and create a *single* shared library `libSPTK.so` or `libSPTK.dylib` (depends on your environment). You can install or uninstall the library with `./waf install` and `./waf uninstall` (library is installed to `usr/local/lib/` by default). To compile your code with the library, you will have to add `` `pkg-config SPTK --cflags --libs` `` as a compiling option.
- Add `swipe` inteface to `SPTK.h` [SPTK.h#L382](https://github.com/r9y9/SPTK/blob/master/include/SPTK.h#L382) (API breaking)
 - refs [e322212](https://github.com/r9y9/SPTK/commit/e322212fccc7342dbae044d64786813c1ad724db) [d50ae4b](https://github.com/r9y9/SPTK/commit/d50ae4b7f54f0e2f2509a0fda36c2d66a9d16a03) [4283d47](https://github.com/r9y9/SPTK/commit/4283d47498988bd7b974b974dd3aa6920c10013a)
- Add python wrapper using SWIG
 - To install the python wrapper, compile the SPTK with `./waf configure --python`. And then `import sptk` will work. [sptk.py](https://github.com/r9y9/SPTK/blob/master/python/sptk.py)
- Windows 32/64 bit support
 - DLLs are avilable on the release page (https://github.com/r9y9/SPTK/releases)

## Supported Platforms

- Linux
- Mac OS X
- Windows

## Installation

Type the following commands from terminal:

```bash
./waf configure
./waf
sudo ./waf install
```

To enable python wrapper, use `./waf configure --python` instread of `./waf configure`.

## Bindings

- [SPTK.jl](https://github.com/r9y9/SPTK.jl) for Julia
- [sptk.py](https://github.com/r9y9/SPTK/blob/master/python/sptk.py) for python
- [gossp](https://github.com/r9y9/gossp/tree/master/3rdparty/sptk) for Golang

## License

- The SPTK is licensed under [BSD](./COPYING)
- The python wrapper is licensed under [MIT](./LICENSE)
