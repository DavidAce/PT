[![Build Status](https://travis-ci.org/DavidAce/PT.svg?branch=master)](https://travis-ci.org/DavidAce/PT)
# Monte Carlo Parallel Tempering


## Requirements
### On Linux:

- C++ compiler (GNU GCC or Clang) with full c++-17 support. 
- Eigen version >= 3.3 (auto-installed by build script if not found)
- HDF5 version >= 1.10 (auto-installed by build script if not found)

### On Mac OS:

- GNU GCC version >= 7, installed through homebrew (brew install gcc). Clang or AppleClang will *NOT* work. This is because the GCC is shipped with a newer standard library implementation (libstdc++) supporting <experimental/*> headers.


