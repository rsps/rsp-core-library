# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.1.0] - 2026-08-26

### Added

- Embedded HTTP client library (`EHttp`), with support for chunked transfer encoding and TLS.
- `TLSSocket` class, wrapping Mbed-TLS for secure socket connections.
- `Base64` encoder.
- `UrlParser`, for splitting a URL into its components.
- `VersionNumber` and `Version` classes, providing semantic version parsing.
- Automatic generation of `version.h` from the `VERSION` file, with Git metadata (tag, commit count, SHA) appended.
- Automatic generation of `config.h`, exposing build-time feature macros to consumers of the library.
- CMake install rules and `rsp-core-libConfig.cmake`, allowing the library to be installed and consumed via `find_package()`.
- Support for building shared libraries (`SOVERSION` is set accordingly); static build remains the default.
- CMake presets for Clang builds and for cross-compiling with `arm-buildroot`.
- CMake `Find` modules for `SDL2` and `wpa_supplicant`.
- Custom enum reflection utility, replacing the `magic_enum` dependency.
- Custom UTF-8/UTF-32 conversion algorithms to avoid deprecated `std::wstring_convert` / `std::codecvt_utf8`.
- `BioBuffer` and `X509Certs` helper classes for certificate handling.
- `GetLibraryVersion()` member function to the `ZLib` wrapper.

### Changed

- Public headers moved to `include/rsp/`.
- Refactored the network stack: `HttpDownload` merged into `HttpRequestBase`; socket reconnect handling moved to `EHttpSession`.
- Refactored CMake dependency handling for tests; 3rd party test dependencies are now fetched via `ExternalProject`.
- `ObjectPool` is now thread safe, requires `T` to be default-constructible, and disallows copying.
- Enabled standard library hardening (`_FORTIFY_SOURCE=2`, stack protector, etc.) for CMake preset builds.
- Upgraded dependencies: `doctest` to v2.5.2, Mbed-TLS to v3.6.6.
- Refactored `ZLib` wrapper to not propagate zlib dependency.

### Fixed

- Out-of-bounds read/write in `Base64::Encode()` and `SocketAddress::GetCanonicalName()`.
- Bug in `ObjectPool::Put()`, causing corruption via `reinterpret_cast`.
- Socket reconnect flow, dangling request objects on exception, and shutdown handling for unconnected sockets.
- Possible overflow when reading from `ChunkStream` at EOF.
- `EHttpRequest` URI formatting, when query and fragment are both present.
- `TLSSocket::Read()` error handling.
- `ZLib` `inflate()` return value is now checked before reading bytes.
- CMake: test binary no longer registered with CTest when cross compiling.

### Removed

- Dependency on `magic_enum` (replaced by custom enum reflection).

## [0.0.0]

### Added

* `install-dev-libraries.sh` script, for installing project's runtime dependencies.
* `rsp-cmake-scripts` as top level project dependency, in `CMakeLists.txt`.
* `VERSION` file in root directory.
* Description and version of top level project, in `CMakeLists.txt`.
* `setup-version.cmake` module.
* `CHANGELOG.md`.

### Changed

* Minimum required CMake version set to `3.30.0`.
* Local module path is now appended to `CMAKE_MODULE_PATH`, instead of "forced" set.
* CMake now fails, if attempted to be built in the same location as the source code.
* Replaced `STRICT_COMPILE_OPTIONS` with `RSP_GCC_STRICT_COMPILE_OPTIONS`, in top level `CMakeLists.txt`.
* Extracted installation of 3rd party dependencies into `dependencies.cmake` and `dev-dependencies.cmake`.
* Refactored top level `CMakeLists.txt`.
* Refactored `tests/CMakeLists.txt`.
* `README.md` now references the newly added `install-runtime.sh` script.

### Fixed

* JSON related tests fail, due to missing [JSON Lint](https://github.com/Seldaek/jsonlint) dev-dependency.

### Deprecated

* `FindMbedTLS.cmake`. Replaced by `external/mbed-tls.cmake` module.

### Removed

* `cmake/strict-compile-options.cmake` (_replaced by `RSP_GCC_STRICT_COMPILE_OPTIONS`, in `rsp-cmake-scripts` package_).


[Unreleased]: https://github.com/rsps/rsp-core-library/compare/v0.1.0...development
[0.1.0]: https://github.com/rsps/rsp-core-library/compare/5e5ad86d...v0.1.0
