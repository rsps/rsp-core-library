# rsp-core-library

Open-source library for medical device software.

This library are being developed following processes adhering to IEC 62304 and IEC 60601-1 chapter 14.
At least one versioned release will be used in a TGM device by RSP Systems and submitted for FDA approval.

Developed by RSP Systems.

## License information

All source files shall include the following file header:

```
/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      <name>
 */
```
**Note:** To add \license tag to doxygen, please see: https://stackoverflow.com/questions/12353409/doxygen-and-license-copyright-informations

## Prerequisites

The library uses libraries for OpenSSL, Curl and FreeType2 when building to a Linux platform.
The libraries should be available for the linker to build the program.

To install those libraries on on Debian based systems, execute the following in a terminal:

```
sudo apt install libssl-dev libcurl4 libcurl4-openssl-dev libfreetype-dev
```

The code is using some C++20 features, so make sure to use a recent compiler to build the project.
To install GCC-10 on Ubuntu 20.04:

```
sudo apt install gcc-10 g++-10
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 --slave /usr/bin/gcov gcov /usr/bin/gcov-10
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 100
```

Also cmake should be at least version 3.18.


## Build and tests

To build the library and accompanied test, simply go into the build directory and run `cmake ..`:

```
cd build
cmake ..
make
```
Tests can now be executed with `./rsp-core-lib-test` or simply `ctest`


