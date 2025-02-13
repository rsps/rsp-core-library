# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

* `rsp-cmake-scripts` as top level project dependency, in `CMakeLists.txt`.
* `VERSION` file in root directory.
* Description and version of top level project, in `CMakeLists.txt`. 
* `CHANGELOG.md`.

### Changed

* Minimum required CMake version set to `3.30.0`.
* Local module path is now appended to `CMAKE_MODULE_PATH`, instead of "forced" set.
* CMake now fails, if attempted to be built in the same location as the source code.
* Replaced `STRICT_COMPILE_OPTIONS` with `RSP_GCC_STRICT_COMPILE_OPTIONS`, in top level `CMakeLists.txt`. 
* Restyled / formatted top level `CMakeLists.txt`.

### Removed

* `cmake/strict-compile-options.cmake` (_replaced by `RSP_GCC_STRICT_COMPILE_OPTIONS`, in `rsp-cmake-scripts` package_).

[Unreleased]: https://github.com/rsps/rsp-core-library/compare/main...development