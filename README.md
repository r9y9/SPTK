# SPTK
-----------------

This repository hosts a modified version of [Speech Signal Processing Toolkit (SPTK)](http://sp-tk.sourceforge.net/) to use the SPTK as **API** (not command line program) from external programs. For command line use, please check [the original one](http://sp-tk.sourceforge.net/).

The important changes from the original SPTK are summerized as follows:

- Integrate [waf](https://code.google.com/p/waf/) build tool
 - `./waf configure && ./waf` compiles all souces in `bin` and `lib` directories and create a *single* shared library `libSPTK.so` or `libSPTK.dylib` (depends on your environment). You can install or uninstall the library with `./waf install` and `./waf uninstall` (library is installed to `usr/local/lib/` by default). Also, you can specify the complier to set the `CC` environmental varaible (e.g. `CC=clang ./waf configure && /waf`). To compile your code with the library, you will have to add `` `pkg-config SPTK --cflags --libs` `` as a compiling option.
- Add `swipe` to `SPTK.h` [SPTK.h#L337](https://github.com/r9y9/SPTK/blob/master/include/SPTK.h#L337) (API breaking)
 - refs [e322212](https://github.com/r9y9/SPTK/commit/e322212fccc7342dbae044d64786813c1ad724db) [d50ae4b](https://github.com/r9y9/SPTK/commit/d50ae4b7f54f0e2f2509a0fda36c2d66a9d16a03) [4283d47](https://github.com/r9y9/SPTK/commit/4283d47498988bd7b974b974dd3aa6920c10013a)
- Add python wrapper using SWIG
 - To install the python wrapper, compile the SPTK with `./waf configure --python`. And then `import sptk` will work. [sptk.py](https://github.com/r9y9/SPTK/blob/master/python/sptk.py)

## Installation

Type the following commands from terminal:

    ./waf configure
    ./waf
    sudo ./waf install

To enable python wrapper, use `./waf configure --python` instread of `./waf configure`.

## Bindings

- Python -> [sptk.py](https://github.com/r9y9/SPTK/blob/master/python/sptk.py)
- Go -> [gossp](https://github.com/r9y9/gossp/tree/master/3rdparty/sptk)
- Julia -> [SPTK.jl](https://github.com/r9y9/SPTK.jl)

## License

- The original SPTK - [BSD](./COPYING)
- Python wrapper - [MIT](./LICENSE)
