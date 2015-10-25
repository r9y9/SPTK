# SPTK

[![Build Status](https://travis-ci.org/r9y9/SPTK.svg?branch=master)](https://travis-ci.org/r9y9/SPTK)
[![Build status](https://ci.appveyor.com/api/projects/status/8y2w0gbvve6anrsn/branch/master?svg=true)](https://ci.appveyor.com/project/r9y9/sptk/branch/master)

This repository hosts a modified version of [Speech Signal Processing Toolkit (SPTK)](http://sp-tk.sourceforge.net/) for embedded programs.

This version of SPTK is used by the following bindings:

- [r9y9/SPTK.jl](https://github.com/r9y9/SPTK.jl)
- [r9y9/pysptk](https://github.com/r9y9/pysptk)
- [r9y9/gossp](https://github.com/r9y9/gossp/tree/master/3rdparty/sptk)

Note that if you want the command line programs of SPTK, please check the original one instead.

## Supported Platforms

- Linux
- Mac OS X
- Windows

## Changes

The important changes from the original SPTK are summerized as follows:

- Integrate [waf](https://code.google.com/p/waf/) build tool
- Add `swipe` inteface to `SPTK.h` [SPTK.h#L382](https://github.com/r9y9/SPTK/blob/master/include/SPTK.h#L382)
- Windows 32/64 bit support. DLLs are avilable on the release page (https://github.com/r9y9/SPTK/releases)
- Avoid `exit` inside a function, as possible.
- Remove `wavsplit` and `wavjoin` from target sources to compile for cross-platform compilation ability, since original `wavsplit` and `wavjoin` use `direct.h` that only exists in POSIX enviroments. ([#8])
- Add `DLLEXPORT` macro to expose API functions explicitly in MSVC environments (NOTE: this is not fully used for now)  ([#8])
- Ensure c89 compatibility
- Export excite function ([#11])
- Cleanup and export RAPT function ([#12])

## Installation

Type the following commands from terminal:

```bash
./waf configure
./waf
sudo ./waf install
```

## License

[Modified BSD](./COPYING)


[#8]: https://github.com/r9y9/SPTK/pull/8
[#11]: https://github.com/r9y9/SPTK/pull/11
[#12]: https://github.com/r9y9/SPTK/pull/12
