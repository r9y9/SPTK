# SPTK

[![Build Status](https://travis-ci.org/r9y9/SPTK.svg?branch=master)](https://travis-ci.org/r9y9/SPTK)
[![Build status](https://ci.appveyor.com/api/projects/status/8y2w0gbvve6anrsn/branch/master?svg=true)](https://ci.appveyor.com/project/r9y9/sptk/branch/master)

This repository hosts a modified version of [Speech Signal Processing Toolkit (SPTK)](http://sp-tk.sourceforge.net/) for embedded programs. For command line use, please check [the original SPTK](http://sp-tk.sourceforge.net/) instead.

The important changes from the original SPTK are summerized as follows:

- Integrate [waf](https://code.google.com/p/waf/) build tool
- Add `swipe` inteface to `SPTK.h` [SPTK.h#L382](https://github.com/r9y9/SPTK/blob/master/include/SPTK.h#L382)
- Windows 32/64 bit support. DLLs are avilable on the release page (https://github.com/r9y9/SPTK/releases)
- Avoid `exit` inside a function, as possible.

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
- [pysptk](https://github.com/r9y9/pysptk) for python
- [gossp](https://github.com/r9y9/gossp/tree/master/3rdparty/sptk) for Golang

## License

- The SPTK is licensed under [BSD](./COPYING)
- The python wrapper is licensed under [MIT](./LICENSE)
